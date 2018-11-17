# include "../incl/hermes.h"

bool                new_tsem(t_sem **sem)
{

	if (!(sem = (t_sem**)memalloc(sizeof(t_sem))))
		return (false);
	return (true);
}

void                tpool_event(t_thread_pool *pool)
{
	uint16_t i;

	i = 0;
	pthread_mutex_lock(&pool->tsem->stop);
	while (i < pool->thread_amt)
	{
		pool->threads[i].working = true;
		i++;
	}
	pthread_cond_broadcast(&pool->tsem->wait);
	pthread_mutex_unlock(&pool->tsem->stop);
	pool->amt_working = i;
}

void                tpool_wait(t_thread_pool *pool)
{
	pthread_mutex_lock(&pool->tsem->stop);
	while (pool->work_pool->total == 0)
		pthread_cond_wait(&pool->tsem->wait, &pool->tsem->stop);
	pthread_mutex_unlock(&pool->tsem->stop);
}

void				tpool_kill(t_thread_pool **pool)
{
	int				i;

	i = -1;
	if (!pool)
		return ;
	while (++i < (*pool)->thread_amt)
	{
		(*pool)->threads[i].alive = false;
	tpool_event(*pool);
	sleep(5);
	if ((*pool)->threads)
		free((*pool)->threads);
}
void					kill_threadpool(t_thread_pool **pool)
{
	int					i;

	i = -1;
	while (++i < (*pool)->thread_amt)
		(*pool)->threads[i].alive = false;
	}
	sleep(1);
	if ((*pool)->threads)
		free((*pool)->threads);
	free(*pool);
}

int						find_interface_indx(int sock)
{
	struct ifreq		ifr;
	char				*ifname;
	char				errbuff[PCAP_ERRBUF_SIZE];

	if (!(ifname = pcap_lookupdev(errbuff)))
		return (hermes_error(FAILURE, "pcap_lookupdev() %s", errbuff));
	strcpy(ifr.ifr_ifrn.ifrn_name, ifname);
	printf("interface: %s\n", ifr.ifr_ifrn.ifrn_name);
	if ((ioctl(sock, SIOCGIFINDEX, &ifr)) < 0)
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
	char			*iface;
	int				timeout;

	timeout = thread->pool->env->opts.init_rtt_timeo;
	if (!(iface = pcap_lookupdev(errbuff)))
		return(hermes_error(FAILURE, "pcap_lookupdev() %s", errbuff));
	if (!(thread->pcaphand = pcap_open_live(iface, 128, 0, timeout, errbuff)))
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
		thread->alive = false;
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
		thread->alive = false;
		return (hermes_error(FAILURE, "setsockopt() PACKET_TX_RING %s", strerror(errno)));
	}
	printf("mmap PACKET_TX_RING success\n");
	memset(&sll_loc, 0, sizeof(struct sockaddr_ll));
	sll_loc.sll_family = AF_PACKET;
	sll_loc.sll_protocol = htons(ETH_P_ALL);
	if ((ifinx = find_interface_indx(thread->sock)) < 0)
	{
		thread->alive = false;
		return (FAILURE);
	}
	printf("found interface index\n");
	sll_loc.sll_ifindex = ifinx;
	/* TODO possibly increase iface mtu */
	if (bind(thread->sock, (sockaddr *) &sll_loc, sizeof(struct sockaddr_ll)) ==
		-1) {
		thread->alive = false;
		return (hermes_error(FAILURE, "bind() %s", strerror(errno)));
	}
	if (!(thread->tx_ring = mmap(0,thread->ring_size, PROT_READ|PROT_WRITE, MAP_SHARED, thread->sock, 0)))
	{
		thread->alive = false;
		return (hermes_error(FAILURE, "mmap() TX_RING %s", strerror(errno)));
	}
	return (SUCCESS);
}

int						prepare_thread_rx_tx(t_thread *thread)
{
	prepare_packetmmap_tx_ring(thread);
	prepare_pcap_rx(thread);
	return (SUCCESS);
}

void					*thread_loop(void *thrd)
{
	t_targetset		work;
	t_thread_pool   *tpool;
	t_thread		*thread;

	thread = (t_thread *)thrd;
	tpool = thread->pool;
	thread->results = memalloc(sizeof(t_result *) * (THRD_HSTGRP_MAX + 64));
	prepare_thread_rx_tx(thread);
	thread->lookup = new_hashtbl(THRD_HSTGRP_MAX);
	memset(&work, 0, sizeof(t_targetset));
	while (thread->alive)
	{
		tpool_wait(tpool);
		pthread_mutex_lock(&thread->pool->work_pool_mtx);
		if (thread->pool->work_pool->total > 0)
		{
			transfer_work(&work, thread->pool->work_pool, thread->amt);
			pthread_mutex_unlock(&thread->pool->work_pool_mtx);
			thread->amt *= (thread->amt < THRD_HSTGRP_MAX) ? 2 : 1;
			if (work.total > 0)
			{
				pthread_mutex_lock(&thread->pool->amt_working_mtx);
				thread->pool->amt_working += 1;
				pthread_mutex_unlock(&thread->pool->amt_working_mtx);
				/* TODO have randomized port option working.*/
				run_scan(thread, &work);
				pthread_mutex_lock(&thread->pool->amt_working_mtx);
				thread->pool->amt_working -= 1;
				pthread_mutex_unlock(&thread->pool->amt_working_mtx);
			}
		}
		else
		{
			pthread_mutex_unlock(&thread->pool->work_pool_mtx);
		}
	}
	return (NULL);
}

t_thread_pool			*init_threadpool(t_env *env, t_targetset *workpool,
											t_resultset *results)
{
	int					i;
	t_thread_pool		*pool;

	i = -1;
	if (!(pool = memalloc(sizeof(t_thread_pool))))
		return (NULL);
	if (!(pool->threads = memalloc(sizeof(t_thread) * env->opts.thread_count)))
		return (NULL);
	if (pthread_mutex_init(&pool->work_pool_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	if (pthread_mutex_init(&pool->results_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	if (pthread_mutex_init(&pool->amt_working_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	pool->thread_amt = env->opts.thread_count;
	pool->reqest_amt = pool->thread_amt;
	if (!(pool->tsem = memalloc(sizeof(t_sem))))
		return (NULL);
	pthread_mutex_init(&pool->work_pool_mtx, NULL);
	pthread_mutex_init(&pool->results_mtx, NULL);
	pthread_mutex_init(&pool->amt_working_mtx, NULL);
	pthread_mutex_init(&pool->tsem->stop, NULL);
	pthread_cond_init(&pool->tsem->wait, NULL);
	pool->thread_amt = env->opts.thread_count;
	pool->reqest_amt = pool->thread_amt;
	pool->results = results;
	pool->work_pool = workpool;
	pool->env = env;
	while (++i < pool->thread_amt)
	{
		pool->threads[i].id = (uint8_t)(i + 2);
		pool->threads[i].amt = 1;
		pool->threads[i].pool = pool;
		pool->threads[i].alive = true;
		pthread_create(&pool->threads[i].thread, NULL, thread_loop, &pool->threads[i]);
	}
	return (pool);
}
