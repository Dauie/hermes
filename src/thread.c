# include "../incl/hermes.h"

void					toggle_thread_alive(t_thread *thread)
{
	pthread_mutex_lock(&thread->pool->amt_alive_mtx);
	printf("thread %d closing early\n", thread->id);
	thread->alive = false;
	thread->pool->amt_alive -= 1;
	pthread_mutex_unlock(&thread->pool->amt_alive_mtx);
}
void					kill_threadpool(t_thread_pool **pool)
{
	(void)pool;
	return ;
}

int						find_interface_indx(t_thread *thread)
{
	struct ifreq		ifr;


	strcpy(ifr.ifr_ifrn.ifrn_name, thread->pool->iface);
	printf("interface: %s\n", ifr.ifr_ifrn.ifrn_name);
	if ((ioctl(thread->sock, SIOCGIFINDEX, &ifr)) < 0)
		return (hermes_error(FAILURE, "ioctl() %s", strerror(errno)));

	return (ifr.ifr_ifindex);
}

/*
** Transmission process is similar to capture as shown below.
**
**[setup]          socket() -------> creation of the transmission socket
**                 setsockopt() ---> allocation of the circular buffer (ring)
**                                   option: PACKET_TX_RING
**                 bind() ---------> bind transmission socket with a network interface
**                 mmap() ---------> mapping of the allocated buffer to the
**                                   user process
**
**[transmission]   poll() ---------> wait for free packets (optional)
**                 send() ---------> send all packets that are set as ready in
**                                   the ring
**                                   The flag MSG_DONTWAIT can be used to return
**                                   before end of transfer.
**
**[shutdown]  close() --------> destruction of the transmission socket and
**                              deallocation of all associated resources.
*/

int						prepare_pcap_rx(t_thread *thread)
{
	char			errbuff[PCAP_ERRBUF_SIZE] = {0};
	int				timeout;

	timeout = thread->pool->env->opts.init_rtt_timeo;

	if (!(thread->pcaphand = pcap_open_live(thread->pool->iface, 128, 0, timeout, errbuff)))
		return (hermes_error(FAILURE, "pcap_open_live() %s", errbuff));
	if (errbuff[0] != 0)
		hermes_error(0, "pcap_open_live() %s", errbuff);
	return (SUCCESS);
}

int						prepare_packetmmap_tx_ring(t_thread *thread)
{
	struct tpacket_req	tpr;
	struct sockaddr_ll	sll_loc;
	int					ifinx;

	/* Step 1 Create PF_PACKET socket. Using SOCK_DGRAM so packets will be "cooked" */
	thread->sock = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
	if (thread->sock == -1)
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "socket() %s", strerror(errno)));
	}
	/* Step 2 determine sizes for PACKET_TX_RING and allocate ring via setsockopt() */
	memset(&tpr, 0, sizeof(struct tpacket_req));
	tpr.tp_block_size = (unsigned int)getpagesize();
	tpr.tp_frame_size = TPACKET_HDRLEN + IP_HDRLEN + DEF_TRAN_HDRLEN + thread->pool->env->cpayload_len;
	tpr.tp_frame_size = pow2_round(tpr.tp_frame_size);
	tpr.tp_block_nr = (THRD_HSTGRP_MAX / (tpr.tp_block_size / tpr.tp_frame_size));
	tpr.tp_frame_nr = tpr.tp_block_nr * (tpr.tp_block_size / tpr.tp_frame_size);
	thread->ring_size = tpr.tp_block_size * tpr.tp_block_nr;
	printf("frame size: %d | blocksize: %d | block count %d | frame count %d\n", tpr.tp_frame_size, tpr.tp_block_size, tpr.tp_block_nr, tpr.tp_frame_nr);

	if (setsockopt(thread->sock, SOL_PACKET, PACKET_TX_RING, (void *)&tpr, sizeof(tpr)) < 0)
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "setsockopt() PACKET_TX_RING %s", strerror(errno)));
	}
	printf("mmap PACKET_TX_RING success\n");
	memset(&sll_loc, 0, sizeof(struct sockaddr_ll));
	sll_loc.sll_family = AF_PACKET;
	sll_loc.sll_protocol = htons(ETH_P_ALL);
	if ((ifinx = find_interface_indx(thread)) < 0)
	{
		toggle_thread_alive(thread);
		return (FAILURE);
	}
	printf("found interface index\n");
	sll_loc.sll_ifindex = ifinx;
	/* TODO possibly increase iface mtu */
	if (bind(thread->sock, (sockaddr *)&sll_loc, sizeof(struct sockaddr_ll)) == -1)
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "bind() %s", strerror(errno)));
	}
	if (!(thread->tx_ring = mmap(0,thread->ring_size, PROT_READ|PROT_WRITE, MAP_SHARED, thread->sock, 0)))
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "mmap() TX_RING %s", strerror(errno)));
	}
	return (SUCCESS);
}

int						prepare_thread_rx_tx(t_thread *thread)
{
	printf("thread %d preparing tx_ring\n", thread->id);
	prepare_packetmmap_tx_ring(thread);
	printf("thread %d prepared tx_ring. doing rx now...\n", thread->id);
	prepare_pcap_rx(thread);
	printf("thread %d finished preparing rx\n", thread->id);
	return (SUCCESS);
}

void					*thread_loop(void *thrd)
{
	t_targetset			work;
	t_thread			*thread;

	thread = (t_thread *)thrd;
	printf("im alive %d\n", thread->id);
	thread->results = memalloc(sizeof(t_result *) * (THRD_HSTGRP_MAX));
	prepare_thread_rx_tx(thread);
	thread->lookup = new_hashtbl(THRD_HSTGRP_MAX);
	printf("got setup %d\n", thread->id);
	while (thread->alive)
	{
		printf("im still alive %d\n", thread->id);
		pthread_mutex_lock(&thread->pool->work_pool_mtx);
		if (thread->pool->workpool->total > 0)
		{
			memset(&work, 0, sizeof(t_targetset));
			transfer_work(&work, thread->pool->workpool, thread->amt);
			pthread_mutex_unlock(&thread->pool->work_pool_mtx);
			if (work.total > 0)
			{
				thread->amt *= (thread->amt < THRD_HSTGRP_MAX) ? 2 : 1;
				pthread_mutex_lock(&thread->pool->amt_working_mtx);
				thread->pool->amt_working += 1;
				thread->working = true;
				pthread_mutex_unlock(&thread->pool->amt_working_mtx);
				/* TODO have randomized port option working.*/
				run_scan(thread, &work);
				pthread_mutex_lock(&thread->pool->amt_working_mtx);
				thread->pool->amt_working -= 1;
				thread->working = false;
				pthread_mutex_unlock(&thread->pool->amt_working_mtx);
			}
		}
		else
		{
			pthread_mutex_unlock(&thread->pool->work_pool_mtx);
			sleep(1);
		}
	}
	free(thread->results);
	free(thread->lookup->buckets);
	free(thread->lookup);
	pcap_close(thread->pcaphand);
	pthread_mutex_lock(&thread->pool->amt_alive_mtx);
	thread->pool->amt_alive -= 1;
	printf("thread %d closing %d still alive\n", thread->id, thread->pool->amt_alive);
	pthread_mutex_unlock(&thread->pool->amt_alive_mtx);
	return (NULL);
}



t_thread_pool			*init_threadpool(t_env *env, t_targetset *workpool,
											t_resultset *results)
{
	char				errbuff[PCAP_ERRBUF_SIZE];
	int					i;
	t_thread_pool		*pool;

	i = -1;
	if (!(pool = memalloc(sizeof(t_thread_pool))))
		return (NULL);
	pool->thread_amt = env->opts.thread_count;
	if (!(pool->threads = memalloc(sizeof(t_thread) * pool->thread_amt)))
		return (NULL);
	pool->reqest_amt = pool->thread_amt;
	pool->results = results;
	pool->workpool = workpool;
	pool->env = env;
	if (pthread_mutex_init(&pool->work_pool_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	if (pthread_mutex_init(&pool->results_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	if (pthread_mutex_init(&pool->amt_working_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	if (pthread_mutex_init(&pool->amt_alive_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	if (!(pool->iface = pcap_lookupdev(errbuff)))
	{
		hermes_error(FAILURE, "pcap_lookupdev() %s", errbuff);
		return (NULL);
	}
	while (++i < pool->thread_amt)
	{
		pool->threads[i].id = (uint8_t)(i + 1);
		pool->threads[i].amt = 1;
		pool->threads[i].pool = pool;
		pool->threads[i].alive = true;
		pthread_mutex_lock(&pool->amt_alive_mtx);
		pool->amt_alive += 1;
		pthread_mutex_unlock(&pool->amt_alive_mtx);
		if (pthread_create(&pool->threads[i].thread, NULL, thread_loop, &pool->threads[i]) != 0)
		{
			hermes_error(FAILURE, "pthread_create()");
			pool->threads[i].alive = false;
			pthread_mutex_lock(&pool->amt_alive_mtx);
			pool->amt_alive -= 1;
			pthread_mutex_unlock(&pool->amt_alive_mtx);
		}
	}
	return (pool);
}
