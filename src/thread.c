# include "../incl/hermes.h"

void					toggle_thread_alive(t_thread *thread)
{
	printf("thread %d closing early\n", thread->id);
	thread->alive = false;

}
void					kill_threadpool(t_thread_pool **pool)
{
	(void)pool;
	return ;
}

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

	if (!(thread->rxfilter.handle = pcap_open_live(thread->pool->iface.name, THRD_HSTGRP_MAX, 0, timeout, errbuff)))
		return (hermes_error(FAILURE, "pcap_open_live() %s", errbuff));
	if (errbuff[0] != 0)
		hermes_error(0, "pcap_open_live() %s", errbuff);
	return (SUCCESS);
}

int						prepare_packetmmap_tx_ring(t_thread *thread)
{
	uint32_t			len;
	int					tpacket_v;
	struct sockaddr_ll	sll_loc;

	/* Step 1 Create PF_PACKET socket. Using SOCK_DGRAM so packets will be "cooked" */
	thread->sock = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
	if (thread->sock == -1)
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "socket() %s", strerror(errno)));
	}
	tpacket_v = TPACKET_V1;
#ifndef TPACKET_V2
	tpacket_v = TPACKET_V2;
	if (setsockopt(thread->sock, SOL_PACKET, PACKET_VERSION, &tpacket_v, sizeof(tpacket_v)) < 0)
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "setsockopt() PACKET_VERSION %s", strerror(errno)));
	}
#endif
	len = sizeof(thread->txring.hdrlen);
	if (getsockopt(thread->sock, SOL_PACKET, PACKET_HDRLEN, &thread->txring.hdrlen, &len) < 0)
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "getsockopt() PACKET_HDRLEN %s", strerror(errno)));
	}
	printf("packet hdrlen %d", thread->txring.hdrlen);
	/* Step 3 determine sizes for PACKET_TX_RING and allocate txring via setsockopt() */
	thread->txring.tpr.tp_block_size = (unsigned int)getpagesize();
	printf("%d\n", thread->txring.hdrlen + IP_HDRLEN + DEF_TRAN_HDRLEN);
	thread->txring.tpr.tp_frame_size = thread->txring.hdrlen + IP_HDRLEN + DEF_TRAN_HDRLEN + thread->pool->env->cpayload_len;
	thread->txring.tpr.tp_frame_size = pow2_round(thread->txring.tpr.tp_frame_size);
	thread->txring.tpr.tp_block_nr = (THRD_HSTGRP_MAX / (thread->txring.tpr.tp_block_size / thread->txring.tpr.tp_frame_size));
	thread->txring.tpr.tp_frame_nr = thread->txring.tpr.tp_block_nr * (thread->txring.tpr.tp_block_size / thread->txring.tpr.tp_frame_size);
	thread->txring.size = thread->txring.tpr.tp_block_size * thread->txring.tpr.tp_block_nr;
	thread->txring.doffset = thread->txring.hdrlen - sizeof(struct sockaddr_ll);
	printf("frame size: %d | blocksize: %d | block count %d | frame count %d\n", thread->txring.tpr.tp_frame_size, thread->txring.tpr.tp_block_size, thread->txring.tpr.tp_block_nr, thread->txring.tpr.tp_frame_nr);

	if (setsockopt(thread->sock, SOL_PACKET, PACKET_TX_RING, (void *)&thread->txring.tpr, sizeof(thread->txring.tpr)) < 0)
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "setsockopt() PACKET_TX_RING %s", strerror(errno)));
	}
	printf("mmap PACKET_TX_RING success\n");

	/* Step 4 actually map ring to user space */
	if (!(thread->txring.ring = mmap(0,thread->txring.size, PROT_READ|PROT_WRITE, MAP_SHARED, thread->sock, 0)))
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "mmap() TX_RING %s", strerror(errno)));
	}
	/* TODO possibly increase iface mtu */
	memset(&sll_loc, 0, sizeof(struct sockaddr_ll));
	printf("found interface index\n");
	sll_loc.sll_ifindex = thread->pool->iface.inx;
	sll_loc.sll_family = AF_PACKET;
	sll_loc.sll_protocol = htons(ETH_P_ALL);
	/* Bind our socket to the interface */
	if (bind(thread->sock, (sockaddr *)&sll_loc, sizeof(struct sockaddr_ll)) == -1)
	{
		toggle_thread_alive(thread);
		return (hermes_error(FAILURE, "bind() %s", strerror(errno)));
	}
	/* Get default gateway's MAC address so threads have a destination */

	uint8_t		dstaddr[ETH_ALEN] = {0xf, 0xf, 0xf, 0xf, 0xf, 0xf};
	thread->txring.peer_ll.sll_ifindex = thread->pool->iface.inx;
	thread->txring.peer_ll.sll_protocol = htons(ETH_P_ALL);
	thread->txring.peer_ll.sll_family = AF_PACKET;
	thread->txring.peer_ll.sll_halen = ETH_ALEN;
	memcpy(thread->txring.peer_ll.sll_addr, dstaddr, ETH_ALEN);
	return (SUCCESS);
}

int						prepare_thread_rx_tx(t_thread *thread)
{
	printf("thread %d preparing ring\n", thread->id);
	prepare_packetmmap_tx_ring(thread);
	printf("thread %d prepared ring. doing rx now...\n", thread->id);
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
	thread->hstgrp = memalloc(sizeof(t_host) * (THRD_HSTGRP_MAX));
	prepare_thread_rx_tx(thread);
	thread->lookup = new_hashtbl(THRD_HSTGRP_MAX);
	printf("got setup %d\n", thread->id);
	while (thread->alive)
	{
		printf("im still alive %d\n", thread->id);
		pthread_mutex_lock(&thread->pool->work_mtx);
		if (thread->pool->work->total > 0)
		{
			memset(&work, 0, sizeof(t_targetset));
			transfer_work(&work, thread->pool->work, thread->amt);
			pthread_mutex_unlock(&thread->pool->work_mtx);
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
	int					sock;
	struct ifreq		ifr;
	char				errbuff[PCAP_ERRBUF_SIZE];

	/* Make socket for use with ioctl and to make ARP request */
	if ((sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
		return (hermes_error(FAILURE, "socket() %s", strerror(errno)));
	/* Get the name of an interface we can send on */
	if (!(pool->iface.name = pcap_lookupdev(errbuff)))
	{
		return (hermes_error(FAILURE, "pcap_lookupdev() %s", errbuff));
	}
	memset(&ifr, 0 , sizeof(struct ifreq));
	strcpy(ifr.ifr_name, pool->iface.name);
	printf("ifr name: %s\n", ifr.ifr_name);
	/* Get our interfaces index */
	if ((ioctl(sock, SIOCGIFINDEX, &ifr)) < 0)
		return (hermes_error(FAILURE, "ioctl() SIOCGIFINDEX %s", strerror(errno)));
	pool->iface.inx = ifr.ifr_ifindex;
	if ((ioctl(sock, SIOCGIFHWADDR, &ifr)) < 0)
		return (hermes_error(FAILURE, "ioctl() SIOCGIFHWADDR %s", strerror(errno)));
	memcpy(pool->iface.if_hwaddr, &ifr.ifr_hwaddr, ETH_ALEN);
	printf("got our mac address: ");
	for (int i = 0; i < ETH_ALEN; i++)
	{
		printf("%02x:", pool->iface.if_hwaddr[i]);
	}
	if ((ioctl(sock, SIOCGIFADDR, &ifr)) < 0)
		return (hermes_error(FAILURE, "ioctl() SIOCGIFADDR %s", strerror(errno)));
	memcpy(&pool->iface.if_ip, &ifr.ifr_addr, sizeof(struct sockaddr));
	printf(" ip: %s", inet_ntoa(pool->iface.if_ip));
	if ((ioctl(sock, SIOCGIFDSTADDR, &ifr)) < 0)
		return (hermes_error(FAILURE, "ioctl() SIOCGIFDSTADDR %s", strerror(errno)));
	memcpy(&pool->iface.gw_ip, &ifr.ifr_dstaddr, sizeof(struct sockaddr));
	printf("gw addr %s", inet_ntoa(pool->iface.gw_ip));
	printf("\n");
	return (SUCCESS);
}


t_thread_pool			*init_threadpool(t_env *env, t_targetset *workpool,
											t_resultset *results)
{
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
	pool->work = workpool;
	pool->env = env;
	if (pthread_mutex_init(&pool->work_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	if (pthread_mutex_init(&pool->results_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	if (pthread_mutex_init(&pool->amt_working_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	if (pthread_mutex_init(&pool->amt_alive_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	prepare_interface(pool);
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
