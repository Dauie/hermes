# include "../incl/hermes.h"


void					kill_threadpool(t_thread_pool **pool)
{
	int					i;

	i = -1;
	if (!pool)
		return ;
	while (++i < (*pool)->thread_amt)
	{
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
	if (bind(thread->sock, (sockaddr *)&sll_loc, sizeof(struct sockaddr_ll)) == -1)
	{
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

int						make_rx_filter(t_thread *thread, t_targetset *work)
{
	t_ip4				*ipp;
	t_ip4rng			*rng;
	t_node				*member;
	t_ip4				ip;
	char				bpfexp[2048] = {0};
	int					i;

	i = 0;
	if (work->ips)
	{
		member = work->ips;
		while (member)
		{
			ipp = member->data;
			i += sprintf(&bpfexp[i], "src host %s ", inet_ntoa(*ipp));
			member = member->right;
			if (member)
				i += sprintf(&bpfexp[i], "and ");
		}
	}
	if (work->iprngs)
	{
		member = work->iprngs;
		while (member)
		{
			rng = member->data;
			ip.s_addr = rng->start;
			while (ip4_cmp(&ip.s_addr, &rng->end) <= 0)
			{
				i += sprintf(&bpfexp[i], "src host %s ", inet_ntoa(ip));
				ip.s_addr = ip4_increment(ip.s_addr, 1);
				if (ip4_cmp(&ip.s_addr, &rng->end) <= 0)
					i += sprintf(&bpfexp[i], "and ");
			}
			member = member->right;
			if (member)
				i += sprintf(&bpfexp[i], "and ");
		}
	}
	if (pcap_compile(thread->pcaphand, &thread->filter, bpfexp, 0, PCAP_NETMASK_UNKNOWN) == -1)
		return (hermes_error(FAILURE, "pcap_compile() %s", pcap_geterr(thread->pcaphand)));
	if (pcap_setfilter(thread->pcaphand, &thread->filter) == -1)
		return (hermes_error(FAILURE, "pcap_setfilter() %s", pcap_geterr(thread->pcaphand)));
	return (SUCCESS);
}

void					*thread_loop(void *thrd)
{
	t_targetset			work;
	t_thread			*thread;

	thread = (t_thread *)thrd;
	prepare_thread_rx_tx(thread);
	memset(&work, 0, sizeof(t_targetset));
	while (thread->alive)
	{
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
				make_rx_filter(thread, &work);
				/* TODO flatten port lists into arrays, randomized or
				 * TODO not, then start scan. Scan needs to fill tx_ring and
				 * TODO hit send() then call pcap_loop(), and process its
				 * TODO return values*/
				test_run_scan(thread->pool->env, &work, thread->pool->results,
						&thread->pool->results_mtx);
				pthread_mutex_lock(&thread->pool->amt_working_mtx);
				thread->pool->amt_working -= 1;
				pthread_mutex_unlock(&thread->pool->amt_working_mtx);
			}
		}
		else
		{
			pthread_mutex_unlock(&thread->pool->work_pool_mtx);
			sleep(1);
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
