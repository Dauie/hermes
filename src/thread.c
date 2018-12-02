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
	while (pool->work->total == 0)
		pthread_cond_wait(&pool->tsem->wait, &pool->tsem->stop);
	pthread_mutex_unlock(&pool->tsem->stop);
}

void					toggle_thread_alive(t_thread *thread)
{
	printf("thread %d closing early\n", thread->id);
	thread->alive = false;

}

void				tpool_kill(t_thread_pool **pool)
{
	int				i;

	i = -1;
	if (!pool)
		return ;
	while (++i < (*pool)->thread_amt)
		(*pool)->threads[i].alive = false;
	tpool_event(*pool);
	sleep(5);
	if ((*pool)->threads)
		free((*pool)->threads);
}

/*
void					kill_threadpool(t_thread_pool **pool)
{
	(void)pool;
	return ;
}
*/

/*
** Transmission process is similar to capture as shown below.
**
**[setup]          socket() -------> creation of the transmission socket
**                 setsockopt() ---> allocation of the circular buffer (txring)
**                                   option: PACKET_TX_RING
**                 bind() ---------> bind transmission socket with a network interface
**                 mmap() ---------> mapping of the allocated buffer to the
**                                   user process
**
**[transmission]   poll() ---------> wait for free packets (optional)
**                 send() ---------> send all packets that are set as ready in
**                                   the txring
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
	printf("rtt timeo %d", thread->pool->env->opts.init_rtt_timeo);
	if (!(thread->rxfilter.handle = pcap_open_live(thread->pool->iface.name, PCAP_SNAPLEN, 0, timeout, errbuff)))
		return (hermes_error(FAILURE, "pcap_open_live() %s", errbuff));
	if (errbuff[0] != 0)
		hermes_error(FAILURE, "pcap_open_live() %s", errbuff);
	if ((thread->rxfilter.fd.fd = pcap_get_selectable_fd(thread->rxfilter.handle)) < 0)
		return (hermes_error(FAILURE, "pacp_get_selectable_fd() interface does not support polling"));
//	if (pcap_setnonblock(thread->rxfilter.handle, true, errbuff) != true)
//		return (hermes_error(FAILURE, "pcap_setnonblock() %s", errbuff));
	return (SUCCESS);
}

int							extra_sock_opts(int sock, uint32_t size)
{
	uint32_t				cursz;
	socklen_t				len;

	len = sizeof(cursz);
	if (getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &cursz, &len) < 0)
		return (hermes_error(FAILURE, "getsockopt SO_SNDBUF"));
	if (cursz < size)
	{
		if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) < 0)
			hermes_error(FAILURE, "setsockopt() SO_SNDBUF");
	}
	/* queue disc bypass */
//	const int32_t sock_disc_byp = 1;
//	if (setsockopt(sock, SOL_PACKET, PACKET_QDISC_BYPASS, &sock_disc_byp, sizeof(sock_disc_byp)) < 0)
//	{
//		return (hermes_error(FAILURE, "setsockopt() PACKET_QDISC_BYPASS"));
//	}
	/* sock discard */
//	const int32_t  sock_discard = 1;
//	if (setsockopt(sock, SOL_PACKET, PACKET_LOSS, &sock_discard, sizeof(sock_discard)) < 0)
//	{
//		return (hermes_error(FAILURE, "setsockopt() PACKET_LOSS"));
//	}
	return (SUCCESS);
}


int						prepare_packetmmap_tx_ring(t_thread *thread)
{
	int					tpacket_v;
	struct sockaddr_ll	sll_loc;

	/* Step 1 Create PF_PACKET socket.*/
	thread->sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (thread->sock == -1)
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "socket() %s", strerror(errno)));
	}

	/* Set packet_mmap version */
	tpacket_v = TPACKET_V3;
	if (setsockopt(thread->sock, SOL_PACKET, PACKET_VERSION, &tpacket_v, sizeof(tpacket_v)) < 0)
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "setsockopt() PACKET_VERSION %s", strerror(errno)));
	}

	/* Determine sizes for PACKET_TX_RING and allocate txring via setsockopt() */
	thread->txring.tpr.tp_block_size = (uint)getpagesize();
	thread->txring.tpr.tp_frame_size = TPACKET3_HDRLEN + sizeof(struct ethhdr) + sizeof(struct iphdr) + DEF_TRAN_HDRLEN + thread->pool->env->cpayload_len;
	thread->txring.tpr.tp_frame_size = pow2_round(thread->txring.tpr.tp_frame_size);
	thread->txring.tpr.tp_block_nr = (THRD_HSTGRP_MAX / (thread->txring.tpr.tp_block_size / thread->txring.tpr.tp_frame_size));
	thread->txring.tpr.tp_frame_nr = thread->txring.tpr.tp_block_nr * (thread->txring.tpr.tp_block_size / thread->txring.tpr.tp_frame_size);
	thread->txring.size = thread->txring.tpr.tp_block_size * thread->txring.tpr.tp_block_nr;
	thread->txring.doffset = sizeof(struct tpacket3_hdr);
	if (setsockopt(thread->sock, SOL_PACKET, PACKET_TX_RING, (void *)&thread->txring.tpr, sizeof(thread->txring.tpr)) < 0)
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "setsockopt() PACKET_TX_RING %s", strerror(errno)));
	}
	printf("frame size: %d | blocksize: %d | block count %d | frame count %d\n", thread->txring.tpr.tp_frame_size, thread->txring.tpr.tp_block_size, thread->txring.tpr.tp_block_nr, thread->txring.tpr.tp_frame_nr);
	extra_sock_opts(thread->sock, thread->txring.size);

	/* Map ring to user space */
	if ((thread->txring.ring = mmap(0, thread->txring.size,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			thread->sock, 0)) == MAP_FAILED)
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "mmap() TX_RING %s", strerror(errno)));
	}

	/* Bind our socket to the interface */
	memset(&sll_loc, 0, sizeof(struct sockaddr_ll));
	memcpy(&sll_loc.sll_addr, thread->pool->iface.if_hwaddr, ETH_ALEN);
	sll_loc.sll_ifindex = thread->pool->iface.inx;
	sll_loc.sll_family = AF_PACKET;
	sll_loc.sll_protocol = htons(ETH_P_ALL);
	if (bind(thread->sock, (sockaddr *)&sll_loc, sizeof(struct sockaddr_ll)) == -1)
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "bind() %s", strerror(errno)));
	}
	return (SUCCESS);
}

int						prepare_thread_rx_tx(t_thread *thread)
{
	if (prepare_packetmmap_tx_ring(thread) == FAILURE)
		return (hermes_error(FAILURE, "prepare_packetmmap_tx_ring()"));
	if (prepare_pcap_rx(thread) == FAILURE)
		return (hermes_error(FAILURE, "prepare_pcap_rx()"));
	return (SUCCESS);
}

void					*thread_loop(void *thrd)
{
	t_targetset		    work;
	t_thread		    *thread;

	thread = (t_thread *)thrd;
	if (!(thread->hstgrp = memalloc(sizeof(t_host) * (THRD_HSTGRP_MAX))))
		return ((void *)(uint64)hermes_error(FAILURE, "memalloc() %s", strerror(errno)));

	if (!(thread->lookup = new_hashtbl(THRD_HSTGRP_MAX)))
		return ((void *)(uint64)hermes_error(FAILURE, "new_hashtable() %s", strerror(errno)));

	for (int i = 0; i < THRD_HSTGRP_MAX; i++)
		if (!(thread->hstgrp[i].lookup = new_hashtbl(thread->pool->env->ports.total)))
			return ((void *)(uint64)hermes_error(FAILURE, "new_hashtbl() %s", strerror(errno)));
	if (prepare_thread_rx_tx(thread) == FAILURE)
		return ((void *)(uint64)hermes_error(FAILURE, "prepare_thread_tx_rx()"));
	printf("im alive %d\n", thread->id);
	while (thread->alive)
	{
		pthread_mutex_lock(&thread->pool->work_mtx);
		if (thread->pool->work->total > 0)
		{
			memset(&work, 0, sizeof(t_targetset));
			transfer_work(&work, thread->pool->work, thread->reqamt);
			pthread_mutex_unlock(&thread->pool->work_mtx);
			if (work.total > 0)
			{
				thread->reqamt *= (thread->reqamt < THRD_HSTGRP_MAX) ? 2 : 1;
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
			pthread_mutex_unlock(&thread->pool->work_mtx);
			sleep(1);
		}
	}
	free(thread->hstgrp);
	free(thread->lookup->buckets);
	free(thread->lookup);
	pcap_close(thread->rxfilter.handle);
	pthread_mutex_lock(&thread->pool->amt_alive_mtx);
	thread->pool->amt_alive -= 1;
	printf("thread %d closing %d still alive\n", thread->id, thread->pool->amt_alive);
	pthread_mutex_unlock(&thread->pool->amt_alive_mtx);
	return (NULL);
}

int					prepare_interface(t_thread_pool *pool)
{
	char				errbuff[PCAP_ERRBUF_SIZE];

	/* Get the name of an interface we can send on */
	if (!(pool->iface.name = pcap_lookupdev(errbuff)))
		return (hermes_error(FAILURE, "pcap_lookupdev() %s", errbuff));
	if (get_iface_info(&pool->iface) == FAILURE)
		return (hermes_error(FAILURE, "issues finding interface configuration for '%s'", pool->iface.name));
	return (SUCCESS);
}


t_thread_pool			*init_threadpool(t_env *env, t_targetset *workpool,
											t_resultset *results)
{
	int					i;
	t_thread_pool		*pool;

	i = -1;
	if (!(pool = memalloc(sizeof(t_thread_pool))))
		return ((void *)(uint64_t)hermes_error(FAILURE, "malloc() %s", strerror(errno)));
	pool->thread_amt = env->opts.thread_count;
	if (!(pool->threads = memalloc(sizeof(t_thread) * pool->thread_amt)))
		return ((void *)(uint64_t)hermes_error(FAILURE, "malloc() %s", strerror(errno)));
	pool->reqest_amt = pool->thread_amt;
	pool->results = results;
	pool->work = workpool;
	pool->env = env;
	if (pthread_mutex_init(&pool->work_mtx, NULL) != 0)
		return ((void *)(uint64_t)hermes_error(FAILURE, "pthread_mutex_init()"));
	if (pthread_mutex_init(&pool->results_mtx, NULL) != 0)
		return ((void *)(uint64_t)hermes_error(FAILURE, "pthread_mutex_init()"));
	if (pthread_mutex_init(&pool->amt_working_mtx, NULL) != 0)
		return ((void *)(uint64_t)hermes_error(FAILURE, "pthread_mutex_init()"));
	if (pthread_mutex_init(&pool->amt_alive_mtx, NULL) != 0)
		return ((void *)(uint64_t)hermes_error(FAILURE, "pthread_mutex_init()"));
	if (prepare_interface(pool) == FAILURE)
		return ((void *)(uint64_t)hermes_error(FAILURE, "prepare_interface()"));
	while (++i < pool->thread_amt)
	{
		pool->threads[i].id = (uint8_t)(i + 1);
		pool->threads[i].reqamt = 1;
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
