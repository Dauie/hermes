#include "../incl/hermes.h"

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
		x += sprintf(&bpfexp[x], "src host %s ", inet_ntoa(thread->hstgrp[i].result->ip));
		if (i + 1 < total)
			x += sprintf(&bpfexp[x], "and ");
		i++;
	}
	if (pcap_compile(thread->rxfilter.handle, &thread->rxfilter.prog, bpfexp, 0, PCAP_NETMASK_UNKNOWN) == -1)
		return (hermes_error(FAILURE, "pcap_compile() %s", pcap_geterr(thread->rxfilter.handle)));
	if (pcap_setfilter(thread->rxfilter.handle, &thread->rxfilter.prog) == -1)
		return (hermes_error(FAILURE, "pcap_setfilter() %s", pcap_geterr(thread->rxfilter.handle)));
	return (SUCCESS);
}

void				targetset_to_hstgrp(t_targetset *set, t_thread *thread, t_env *env)
{
	int				i;
	t_ip4rng		*rng;
	t_ip4			start;
	t_node			*tmp;

	i = 0;
	tmp = set->ips;
	while (tmp)
	{
		thread->hstgrp[i].result = new_result();
		thread->hstgrp[i].result->ip.s_addr = ((t_ip4*)tmp->data)->s_addr;
		thread->hstgrp[i++].result->portstats = memalloc(sizeof(t_portstat *) * (env->ports.total + 1));
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
			thread->hstgrp[i].result = new_result();
			thread->hstgrp[i].result->ip.s_addr = start.s_addr;
			thread->hstgrp[i++].result->portstats = memalloc(sizeof(t_portstat *) * (env->ports.total + 1));
			start.s_addr = ip4_increment(start.s_addr, 1);
		}
		tmp = tmp->right;
	}
	del_list(&set->iprngs, true);
	thread->hstgrpsz = (uint8_t)set->total;
}

void add_results_to_lookup(t_thread *thread, size_t count)
{
	size_t			i;

	i = 0;
	while (i < count)
	{
		hashtbl_add(thread->lookup, (uint8_t *)&thread->hstgrp[i].result->ip, 4, (void **)&thread->hstgrp[i]);
		i++;
	}
}

void				fill_tx_ring(t_thread *thread, t_frame *ethframe, uint16_t *alive)
{
	struct tpacket2_hdr *frame;
	void				*data;
	uint16_t			*srcports;
	uint16_t			*dstports;
	uint				ring_i;
	uint				hst_i;

	ring_i = 0;
	hst_i = 0;
	srcports = thread->pool->env->ports.flat;
	dstports = thread->pool->env->dstports;
	while (hst_i < thread->hstgrpsz && ring_i < thread->txring.tpr.tp_frame_nr)
	{
		frame = thread->txring.ring + (thread->txring.tpr.tp_frame_size * ring_i);
		data = (void *)frame + thread->txring.doffset; /* check data offset*/
		if (frame->tp_status == TP_STATUS_WRONG_FORMAT)
			hermes_error(FAILURE, "TX_RING wrong format in frame %i of thread %d", ring_i++, thread->id);
		else if (frame->tp_status == TP_STATUS_LOSING)
			hermes_error(FAILURE, "issue detected during send TX_RING frame %i of thread %d", ring_i++, thread->id);
		else if (frame->tp_status == TP_STATUS_AVAILABLE)
		{
			if (thread->hstgrp[hst_i].health.portinx < thread->pool->env->ports.total)
			{
				ethframe->ip->daddr = thread->hstgrp[hst_i].result->ip.s_addr;
				ip_checksum(ethframe->ip);
				ethframe->tcp->source = htons(dstports[thread->hstgrp[hst_i].health.portinx]);
				ethframe->tcp->dest = htons(srcports[thread->hstgrp[hst_i].health.portinx]);
				tcp_checksum(ethframe->ip, (uint16_t *)ethframe->tcp);
				memcpy(data, ethframe->buffer, ethframe->size);
				frame->tp_len = ethframe->size;
				frame->tp_status = TP_STATUS_SEND_REQUEST;
				thread->hstgrp[hst_i++].health.portinx += 1;
				ring_i++;
			}
			else
			{
				*alive -= 1;
				hst_i++;
			}
		}
		else
			ring_i++;
	}
}

void				send_task(t_thread *thread)
{
	ssize_t			btx = 0;
	int				tot = 0;

	btx = send(thread->sock, NULL, 0, MSG_WAITALL);
	if (btx < 0)
		hermes_error(EXIT_FAILURE, "send() %s", strerror(errno));
	else if (btx == 0)
		;
	else
	{
		tot += btx /  sizeof(struct iphdr) + sizeof(struct tcphdr);
		printf("sent %d packets\n", tot);
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

void				init_tcphdr(t_thread *thread, struct tcphdr *tcp)
{
	(void)thread;
	tcp->doff = 6;
	tcp->window = htons(1024);
	tcp->syn = true;
}

void				init_tcpopt_mss(t_tcpopt *opt)
{
	opt->type = TCP_OPT_MSS;
	opt->len = 4;
	opt->val = htons(1460);
}

void				init_ethframe(t_thread *thread, t_frame *frame)
{

	/* below line will change when more scan types are implemented */
	frame->size = sizeof(struct ethhdr) + sizeof(struct iphdr) +
					sizeof(struct tcphdr) + sizeof(t_tcpopt) +
						thread->pool->env->cpayload_len;
	frame->buffer = memalloc(sizeof(uint8_t) * frame->size);
	frame->eth = (struct ethhdr *)frame->buffer;
	frame->ip = (struct iphdr *)((uint8_t *)frame->eth + sizeof(struct ethhdr));
	frame->tcp = (struct tcphdr *)((uint8_t *)frame->ip + sizeof(struct iphdr));
	frame->tcpopt = (t_tcpopt *)((uint8_t *)frame->tcp + sizeof(struct tcphdr));
	init_ethhdr(thread, frame->eth);
	init_iphdr(thread, frame);
	init_tcphdr(thread, frame->tcp);
	init_tcpopt_mss(frame->tcpopt);
}

void				syn_scan(t_thread *thread)
{
	t_frame			frame;
	uint16_t		alive;

	memset(&frame, 0, sizeof(t_frame));
	init_ethframe(thread, &frame);
	alive = thread->hstgrpsz;
	while (alive > 0)
	{
		fill_tx_ring(thread, &frame, &alive);
		send_task(thread);

	}
	free(frame.buffer);
	frame.buffer = NULL;
}

void				run_scan(t_thread *thread, t_targetset *set)
{
	targetset_to_hstgrp(set, thread, thread->pool->env);
	add_results_to_lookup(thread, set->total);
	make_rx_filter(thread, set->total);
	syn_scan(thread);
}
