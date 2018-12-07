#include "../incl/hermes.h"

void		hex_print(uint8_t *blob, size_t len)
{
	bool	nlspace;

	nlspace = false;
	size_t  i;
	printf("\n'");
	for (i = 0; i < len; i++)
	{
		printf("%02x ", blob[i]);
		if (i % 7 == 0)
		{
			printf("%s", nlspace ? "\n" : " " );
			nlspace = !nlspace;
		}
	}
	printf("'\n\n");
}


/* checksum code is borrowed from roman10.net thanks for the explanations! */
static unsigned short compute_checksum(unsigned short *addr, unsigned int count)
{
	register unsigned long sum = 0;
	while (count > 1)
	{
		sum += * addr++;
		count -= 2;
	}
	if(count > 0)
	{
		sum += ((*addr)&htons(0xFF00));
	}
	while (sum>>16)
	{
		sum = (sum & 0xffff) + (sum >> 16);
	}
	sum = ~sum;
	return ((unsigned short)sum);
}

void ip_checksum(struct iphdr* iphdrp){
	iphdrp->check = 0;
	iphdrp->check = compute_checksum((unsigned short*)iphdrp, iphdrp->ihl<<2);
}

void tcp_checksum(struct iphdr *ip, unsigned short *ippayload)
{
	register unsigned long sum;
	uint16_t len;
	struct tcphdr *tcphdrp;

	tcphdrp = (struct tcphdr*)(ippayload);
	len = ntohs(ip->tot_len) - (uint16_t)(ip->ihl<<2);
	sum = 0;
	sum += (ip->saddr>>16)&0xFFFF;
	sum += (ip->saddr)&0xFFFF;
	sum += (ip->daddr>>16)&0xFFFF;
	sum += (ip->daddr)&0xFFFF;
	sum += htons(IPPROTO_TCP);
	sum += htons(len);

	tcphdrp->check = 0;
	while (len > 1) {
		sum += * ippayload++;
		len -= 2;
	}
	if(len > 0) {
		sum += ((*ippayload)&htons(0xFF00));
	}
	while (sum>>16) {
		sum = (sum & 0xffff) + (sum >> 16);
	}
	sum = ~sum;
	tcphdrp->check = (unsigned short)sum;
}

int 					make_rx_filter(t_thread *thread, size_t total)
{
	char				bpfexp[4096] = {0};
	size_t				i;
	int					x;

	i = 0;
	x = 0;

	if (thread->rxfilter.prog.bf_len > 0)
		pcap_freecode(&thread->rxfilter.prog);
	while (i < total)
	{
		x += sprintf(&bpfexp[x], "src %s ", inet_ntoa(thread->hstgrp[i].result->ip));
		if (i + 1 < total)
			x += sprintf(&bpfexp[x], "or ");
		i++;
	}
	if (pcap_compile(thread->rxfilter.handle, &thread->rxfilter.prog, bpfexp, 0, PCAP_NETMASK_UNKNOWN) == -1)
		return (hermes_error(FAILURE, "pcap_compile() %s", pcap_geterr(thread->rxfilter.handle)));
	if (pcap_setfilter(thread->rxfilter.handle, &thread->rxfilter.prog) == -1)
		return (hermes_error(FAILURE, "pcap_setfilter() %s", pcap_geterr(thread->rxfilter.handle)));
	return (SUCCESS);
}

int targetset_to_hstgrp(t_targetset *set, t_thread *thread,
						t_env *env)
{
	int				y;
	void			*hook;
	t_ip4rng		*rng;
	t_ip4			start;
	t_node			*tmp;
	void			*val;

	y = 0;
	tmp = set->ips;
	thread->hstgrpsz = (uint8_t)set->total;
	while (tmp)
	{
		if (!(thread->hstgrp[y].result = new_result()))
			return (hermes_error(FAILURE, "new_result() %s", strerror(errno)));
		thread->hstgrp[y].result->ip.s_addr = ((t_ip4*)tmp->data)->s_addr;
		printf("%s\n", inet_ntoa(thread->hstgrp[y].result->ip));
		printf("ports total %d\n", env->ports.total);
		if (!(thread->hstgrp[y].result->portstats = memalloc(sizeof(t_portstat) * (env->ports.total + 1))))
			return (hermes_error(FAILURE, "memalloc() %s", strerror(errno)));
		for (int x = 0; x < env->ports.total; x++)
		{
			thread->hstgrp[y].result->portstats[x].port = env->ports.flat[x];
			val = &thread->hstgrp[y].result->portstats[x];
			hashtbl_add(thread->hstgrp[y].lookup, env->ports.flat[x], &val);
			if (hashtbl_get(thread->hstgrp[y].lookup, env->ports.flat[x], &hook) == false)
			{
				hermes_error(FAILURE, "not even\n");
				exit(EXIT_FAILURE);
			}
		}
		val = &thread->hstgrp[y];
		if (hashtbl_add(thread->lookup, thread->hstgrp[y].result->ip.s_addr,
					 &val) != true)
		{
			hermes_error(FAILURE, "mother fucking hash table fucking hash table\n");
			exit(EXIT_FAILURE);
		}
		if (hashtbl_get(thread->lookup, thread->hstgrp[y].result->ip.s_addr, &hook) != true)
		{
			hermes_error(FAILURE, "fucking hash table\n");
			exit(EXIT_FAILURE);
		}
		y++;
		tmp = tmp->right;
	}
	del_list(&set->ips, true);
	tmp = set->iprngs;
	while (tmp)
	{
		rng = tmp->data;
		start.s_addr = rng->start;
		while (ip4_cmp(&start, &rng->end) <= 0)
		{
			if (!(thread->hstgrp[y].result = new_result()))
				return (hermes_error(FAILURE, "new_result() %s", strerror(errno)));
			thread->hstgrp[y].result->ip.s_addr = ((t_ip4*)tmp->data)->s_addr;
			if (!(thread->hstgrp[y].result->portstats = memalloc(sizeof(t_portstat) * (env->ports.total + 1))))
				return (hermes_error(FAILURE, "memalloc() %s", strerror(errno)));
			if (!(thread->hstgrp[y].lookup = new_hashtbl(env->ports.total)))
				return (hermes_error(FAILURE, "new_hashtbl() %s", strerror(errno)));
			for (int j = 0; j < env->ports.total; j++)
			{
				thread->hstgrp[y].result->portstats[j].port = env->ports.flat[j];
				hashtbl_add(thread->hstgrp[y].lookup, env->ports.flat[j],
							 (void **)&thread->hstgrp[y].result->portstats[j]);
			}
			hashtbl_add(thread->lookup, thread->hstgrp[y].result->ip.s_addr,
						 (void **)&thread->hstgrp[y]);
			y++;
			start.s_addr = ip4_increment(start.s_addr, 1);
		}
		tmp = tmp->right;
	}
	del_list(&set->iprngs, true);
	return (SUCCESS);
}

void                    pack_tcp(t_thread *thread, t_frame **ethframe)
{
	(*ethframe)->tcp->source = htons(dstports[thread->hstgrp[hst_i].health.portinx]);
	(*ethframe)->tcp->dest = htons(srcports[thread->hstgrp[hst_i].health.portinx]);
	// TODO :
	if (thread->pool->env->opts.bitops.do_bad_checksum)
		tcp_bad_checksum(ethframe->ip, (uint16_t *) ethframe->tcp);
	else
		tcp_checksum(ethframe->ip, (uint16_t *) ethframe->tcp);
}

int						fill_tx_ring(t_thread *thread, t_frame *ethframe)
{
    /*
     * TODO : comment this function
     */
	struct tpacket3_hdr	*frame;
	void				*data;
	uint16_t			*srcports;
	uint16_t			*dstports;
	uint				ring_i;
	uint				hst_i;
	int					ret;

	ring_i = 0;
	hst_i = 0;
	ret = 0;
	srcports = thread->pool->env->ports.flat;
	dstports = thread->pool->env->dstports;
	while (hst_i < thread->hstgrpsz && ring_i < thread->txring.tpr.tp_frame_nr)
	{
		if (thread->hstgrp[hst_i].health.done == true)
		{
			hst_i++;
			continue;
		}
		frame = (struct tpacket3_hdr *)(thread->txring.ring + (thread->txring.tpr.tp_frame_size * ring_i));
        printf("frame addr || %p\n", frame);
        switch ((volatile uint32_t)frame->tp_status)
		{
			case TP_STATUS_WRONG_FORMAT:
				return (hermes_error(FAILURE, "TX_RING wrong format in frame %i of thread %d", ring_i, thread->id));
			case TP_STATUS_AVAILABLE:
				data = (void*)frame + thread->txring.doffset;
				ethframe->ip->daddr = thread->hstgrp[hst_i].result->ip.s_addr;
				ip_checksum(ethframe->ip);

				if (ethframe->proto_opt == SOCK_STREAM)
					pack_tcp(thread, &ethframe);
				else if (ethframe->proto_opt == SOCK_DGRAM)
					pack_udp(thread, &ethframe);

				memcpy(data, ethframe->buffer, ethframe->size);
				frame->tp_next_offset = 0;
				frame->tp_len = ethframe->size;
				frame->tp_status = TP_STATUS_SEND_REQUEST;
				printf("setup a frame\n");
				ret++; hst_i++; ring_i++;
				break;
			default:
				printf("skipped ring with status %d\n", frame->tp_status);
				ring_i++;
				break;
		}
	}
	return (ret);
}

void				send_task(t_thread *thread)
{
	ssize_t			btx = 0;
	int				totpkt = 0;

	int errn = errno;
	btx = sendto(thread->sock, NULL, 0, 0, NULL, 0);
	printf("btx %lu\n", btx);
	if (btx < 0)
		hermes_error(EXIT_FAILURE, "send() %s", strerror(errno));
	else if (btx == 0)
	{
		printf("didn't send anything.\n");
	}
	else
	{
		totpkt += btx / (sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct tcphdr));
		printf("sent %d packets\n", totpkt);
	}
	if (errn != errno)
	{
		printf("issues: %s", strerror(errno));
	}
}

void				init_ethhdr(t_thread *thread, struct ethhdr *eth)
{
	eth->h_proto = htons(ETH_P_IP); /* possibly ETH_P_IP */
	memcpy(eth->h_dest, thread->pool->iface.gw_hwaddr, ETH_ALEN);
	memcpy(eth->h_source, thread->pool->iface.if_hwaddr, ETH_ALEN);
}

void				init_iphdr(t_thread *thread, t_frame *frame)
{
	frame->ip->version = 4;
	frame->ip->ihl = 5;
	frame->ip->tot_len = htons(frame->size - sizeof(struct ethhdr));
	frame->ip->ttl = thread->pool->env->opts.ip_ttl;
	frame->ip->protocol = IPPROTO_TCP;
	frame->ip->saddr = thread->pool->iface.if_ip.s_addr; /* already in nbo */
}

void				init_udphdr(t_thread *thread, struct udphdr *udp)
{
	(void)thread;
	(void)udp;
}

void				init_tcphdr(t_thread *thread, struct tcphdr *tcp)
{
	(void)thread;
	tcp->seq = htonl(1);
	tcp->ack = 0;
	tcp->doff = 5; // TODO : make this dynamic
	tcp->window = htons(1024);
	tcp->syn = true;
}

void				init_tcpopt_mss(t_tcpopt *opt)
{
	opt->type = TCP_OPT_MSS;
	opt->len = 4;
	opt->val = htons(1460);
}

void				init_ethframe(t_thread *thread, t_frame *frame, int proto)
{

	/* below line will change when more scan types are implemented */
	frame->size = sizeof(struct ethhdr) + sizeof(struct iphdr) +
					sizeof(struct tcphdr) /*+ sizeof(t_tcpopt)*/ +
						thread->pool->env->cpayload_len;
	frame->buffer = memalloc(sizeof(uint8_t) * frame->size);
	frame->eth = (struct ethhdr *)frame->buffer;
	frame->ip = (struct iphdr *)((uint8_t *)frame->eth + sizeof(struct ethhdr));
	if (proto == SOCK_STREAM)
	{
		printf("proto is TCP\n");
		frame->tcp = (struct tcphdr *)((uint8_t *)frame->ip + sizeof(struct iphdr));
		init_tcphdr(thread, frame->tcp);
	}
	else if (proto == SOCK_DGRAM)
	{
		frame->udp = (struct udphdr*)((uint8_t*)frame->ip + sizeof(struct iphdr));
		init_udphdr(thread, frame->udp);
	}
//	frame->tcpopt = (t_tcpopt *)((uint8_t *)frame->tcp + sizeof(struct tcphdr));
	init_ethhdr(thread, frame->eth);
	init_iphdr(thread, frame);
//	init_tcpopt_mss(frame->tcpopt);
}

void	handle_packet(u_char *user, const struct pcap_pkthdr *hdr, const u_char *data)
{
	struct tcphdr	*tcp;
	t_portstat 		*stat;
	void			*bkt_data;
	t_host			*host;
	struct iphdr	*ip;
	uint16_t		*ports;
	t_thread		*thread;

	thread = (t_thread *)user;
	ports = thread->pool->env->ports.flat;
	ip = (struct iphdr *)(data + sizeof(struct ethhdr));
	tcp = (struct tcphdr *)((uint8_t *)ip + sizeof(struct iphdr));
	if (hashtbl_get(thread->lookup, ip->saddr, &bkt_data) == true)
	{
		host = (t_host *)bkt_data;
		tcp->source = ntohs(tcp->source);
		if (tcp->source == ports[host->health.portinx])
		{
			if (hashtbl_get(host->lookup, tcp->source, (void **)&stat) == true)
			{
				stat->status = STAT_OPEN;
				if (host->health.portinx + 1 == thread->pool->env->ports.total)
				{
					host->health.done = true;
					thread->scancnt -= 1;
				}
				printf("packet processed\n");
				host->health.portinx++;
			}
		}
	}

	hex_print((uint8_t *)data, hdr->caplen);
}

long					timediff_ms(struct timeval *then, struct timeval *now)
{
	long		x;

	x = (long)(then->tv_sec - now->tv_sec) * 1000L +
			(long)(then->tv_usec - now->tv_usec) / 1000L;
	return (x);
}

void					syn_scan(t_thread *thread)
{
	struct timeval		sent = {0};
	struct timeval		now = {0};
	long ms;
	int					ret;
	t_frame				frame;

	if (!thread)
		return ;
	memset(&frame, 0, sizeof(t_frame));
	init_ethframe(thread, &frame, SOCK_STREAM);
	thread->scancnt = thread->hstgrpsz;
	while (thread->scancnt > 0)
	{
		gettimeofday(&now, NULL);
		if ((ms = timediff_ms(&now, &sent)) >= DEF_INIT_RTT_TIMEOUT)
		{
			fill_tx_ring(thread, &frame);
			send_task(thread);
			gettimeofday(&sent, NULL);
			thread->rxfilter.fd.events = POLL_IN;
			if ((ret = poll(&thread->rxfilter.fd, 1, DEF_INIT_RTT_TIMEOUT)) < 0)
				hermes_error(FAILURE, "poll() %s", strerror(errno));
			if (ret > 0)
				pcap_dispatch(thread->rxfilter.handle,
							  thread->hstgrpsz, handle_packet, (u_char*)thread);
			else
				printf("didn't get anything\n");
		}
		else
		{
			usleep((useconds_t) ms * 1000);
		}

	}
	free(frame.buffer);
	frame.buffer = NULL;
}

void				run_scan(t_thread *thread, t_targetset *set, void (*scan)(t_thread *thread))
{
	targetset_to_hstgrp(set, thread, thread->pool->env);
	make_rx_filter(thread, set->total);
	printf("scanning\n");
	if (!scan || !*scan)
		hermes_error(FAILURE, "null scan function");
	else
		scan(thread);
}
