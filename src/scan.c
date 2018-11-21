#include "../incl/hermes.h"

void				test_run_scan2(t_thread *thread, t_targetset *set)
{
//	t_result		*tmp;

	targetset_to_hstgrp(set, thread, thread->pool->env);
	add_results_to_lookup(thread, set->total);
	make_rx_filter(thread, set->total);

/* for testing */
	for (size_t i = 0; i < set->total; i++)
	{
		if (thread->hstgrp[i].result->portstats)
		{
			free(thread->hstgrp[i].result->portstats);
			thread->hstgrp[i].result->portstats = NULL;
		}
		if (thread->hstgrp[i].result)
		{
			free(thread->hstgrp[i].result);
			thread->hstgrp[i].result = NULL;
		}
	}
	for (size_t i = 0; i < THRD_HSTGRP_MAX; i++)
	{
		thread->lookup->buckets[i].hash = 0;
		thread->lookup->buckets[i].data = NULL;
	}
	sleep(1);
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

/* Get protocol by name should probably be used. */
/* IP-TTL needs to be set in sanity check */

void				init_txring_tcpsyn(t_thread *thread)
{
	uint32_t			i;
	struct tpacket2_hdr *hdr;
	uint8_t				*data;
	struct tcphdr		tcphdr;
	struct iphdr		iphdr = {
			.ihl = 5,
			.tot_len = IP_HDRLEN + sizeof(struct tcphdr) + thread->pool->env->cpayload_len,
			.version = 4,
			.tos = 0, /* need to do more research on this "diff" */
			.id = 0,
			.frag_off = 0,
			.ttl = thread->pool->env->opts.ip_ttl,
			.protocol = IPPROTO_TCP,
			.saddr = 0, /* verify that kernel fills in */
			.daddr = 0,
	};
	uint8_t			headers[60] = {0};
	i = 0;
	memset(&tcphdr, 0, sizeof(struct tcphdr));
	/* set any initial tcp options */
	tcphdr.syn = true;
	memcpy(headers, &iphdr, sizeof(iphdr));
	memcpy(headers + sizeof(iphdr), &tcphdr, sizeof(tcphdr));
	while (i < thread->txring.tpr.tp_frame_nr)
	{
		hdr = thread->txring.ring + (thread->txring.tpr.tp_frame_size * i);
		data = (void *)hdr + TPACKET_ALIGN(thread->txring.hdrlen);
		memcpy(data, headers, 40);
		i++;
	}
}

void				fill_tx_ring(t_thread *thread, uint16_t *alive)
{
	uint8_t				packet[60] = {0};
	void				*data;
	uint16_t			*srcports;
	uint16_t			*dstports;
	struct tpacket2_hdr *hdr;
	struct iphdr		*ip;
	struct tcphdr		*tcp;
	uint				ring_i;
	uint				hst_i;

	ring_i = 0;
	hst_i = 0;
	/* Init ip header */
	ip = (struct iphdr *)&packet;
	ip->version = 4;
	ip->ihl = 5;
	ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr));
	ip->ttl = thread->pool->env->opts.ip_ttl;
	ip->protocol = IPPROTO_TCP;
	/* Init TCP header */
	tcp = (struct tcphdr *)((uint8_t*)ip + sizeof(struct iphdr));
	srcports = thread->pool->env->ports.flat;
	dstports = thread->pool->env->dstports;
	while (hst_i < thread->hstgrpsz && ring_i < thread->txring.tpr.tp_frame_nr)
	{
		hdr = thread->txring.ring + (thread->txring.tpr.tp_frame_size * ring_i);
		data = (void *)hdr + TPACKET_ALIGN(thread->txring.hdrlen);
		printf("%d", hdr->tp_status);
		if (hdr->tp_status == TP_STATUS_WRONG_FORMAT)
			hermes_error(EXIT_FAILURE, "TX_RING wrong format in frame %i of thread %d", ring_i++, thread->id);
		else if (hdr->tp_status == TP_STATUS_AVAILABLE)
		{
			if (thread->hstgrp[hst_i].health.portinx < thread->pool->env->ports.total)
			{
				ip->daddr = thread->hstgrp[hst_i].result->ip.s_addr;
				tcp->source = htons(dstports[thread->hstgrp[hst_i].health.portinx]);
				tcp->dest = htons(srcports[thread->hstgrp[hst_i].health.portinx]);
				memcpy(data, packet,  sizeof(struct iphdr) + sizeof(struct tcphdr));
				hdr->tp_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
				hdr->tp_status = TP_STATUS_SEND_REQUEST;
				thread->hstgrp[hst_i++].health.portinx += 1;
				ring_i++;
			} else
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

	btx = sendto(thread->sock, NULL, 0, MSG_WAITALL, (sockaddr *)&thread->txring.peer_ll, sizeof(struct sockaddr_ll));
	if (btx < 0)
		hermes_error(EXIT_FAILURE, "sendto() %s", strerror(errno));
	else if (btx == 0)
		;
	else
	{
		tot += btx /  sizeof(struct iphdr) + sizeof(struct tcphdr);
		printf("sent %d packets\n", tot);
	}
}

void				syn_scan(t_thread *thread)
{
	uint16_t		alive;

	alive = thread->hstgrpsz;
//	while (alive > 0)
//	{
	fill_tx_ring(thread, &alive);
	send_task(thread);
	printf("i guess i sent something\n");
//	}
}

void				run_scan(t_thread *thread, t_targetset *set)
{
	targetset_to_hstgrp(set, thread, thread->pool->env);
	add_results_to_lookup(thread, set->total);
	make_rx_filter(thread, set->total);
	syn_scan(thread);
}
