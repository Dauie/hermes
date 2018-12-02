#include "../incl/hermes.h"

static int		get_portlist_from_binnlist(t_node **tree, binn *list)
{
	int			cnt;
	t_port		*port;

	cnt = binn_count(list);
	for (int i = 1; i <= cnt; i++)
	{
		if (!(port = new_port()))
			return (hermes_error(FAILURE, "new_port()"));
		port->port = binn_list_uint16(list, i);
		list_add_head(tree, (void **) &port);
	}
	return (SUCCESS);
}

static int		get_ip4list_from_binnlist(t_node **clist, binn *list)
{
	int			cnt;
	t_ip4		*ip;

	cnt = binn_count(list);
	for (int i = 1; i <= cnt; i++)
	{
		if (!(ip = new_ip4()))
			return (hermes_error(FAILURE, "new_ip4()"));
		ip->s_addr = binn_list_uint32(list, i);
		list_add_head(clist, (void **) &ip);
	}
	return (SUCCESS);
}

static int		get_prtrnglist_from_binnlist(t_node **clst, binn *list)
{
	int			cnt;
	t_prtrng	*rng;
	binn		*obj;

	cnt = binn_count(list);
	for (int i = 1; i <= cnt; i++)
	{
		if (!(rng = new_portrange()))
			hermes_error(FAILURE, "new_portrange()");
		obj = binn_list_object(list, i);
		binn_object_get_uint16(obj, "size", &rng->size);
		binn_object_get_uint16(obj, "start", &rng->start);
		binn_object_get_uint16(obj, "end", &rng->end);
		list_add_head(clst, (void **) &rng);
	}
	return (SUCCESS);
}

static int		get_ip4rnglist_from_binnlist(t_node **clst, binn *list)
{
	int			cnt;
	t_ip4rng	*rng;
	binn		*obj;

	cnt = binn_count(list);
	for (int i = 1; i <= cnt; i++)
	{
		if (!(rng = new_ip4range()))
			return (hermes_error(FAILURE, "new_ip4range()"));
		obj = binn_list_object(list, i);
		binn_object_get_uint32(obj, "size", &rng->size);
		binn_object_get_uint32(obj, "start", &rng->start);
		binn_object_get_uint32(obj, "end", &rng->end);
		list_add_head(clst, (void **) &rng);
	}
	return (SUCCESS);
}

int				get_portstatlist_from_binnlist(binn *list, t_portstat **stats)
{
	int			cnt;
	t_portstat	*pstat;
	binn		*obj;

	cnt = binn_count(list);
	if (!(stats = memalloc(sizeof(t_portstat *) * cnt)))
		return (hermes_error(FAILURE, "malloc() %s", strerror(errno)));
	for (int i = 1; i <= cnt; i++)
	{
		if (!(pstat = new_portstat()))
			return (hermes_error(FAILURE, "new_portstat()"));
		obj = binn_list_object(list, i);
		binn_object_get_uint16(obj, "port", &pstat->port);
		binn_object_get_uint16(obj, "status", &pstat->status);
		stats[i] = pstat;
	}
	return (SUCCESS);
}

int 			get_resultlist_from_binnlist(binn *list, t_node **clist, t_targetset *account)
{
	int			cnt;
	t_result	*res;
	binn		*obj;
	binn		*portstats;

	cnt = binn_count(list);
	for (int i = 1; i <= cnt; i++)
	{
		if (!(res = new_result()))
			return (hermes_error(FAILURE, "new_result()"));
		if (!(obj = binn_list_object(list, i)))
			return (hermes_error(FAILURE, "binn_list_object()"));
		binn_object_get_uint32(obj, "ip", &res->ip.s_addr);
		remove_ip_targetset(account, res->ip.s_addr);
		if (binn_object_get_list(obj, "port_stats", (void **)&portstats) == true)
		{
			get_portstatlist_from_binnlist(portstats, &res->portstats); /* taking the address of portstats to shut the compiler up, need to fix this.*/
		}
		list_add_head(clist, (void **) &res);
	}
	return (SUCCESS);
}

void			unbinnify_resultset(t_resultset *set, t_targetset *work,  binn *obj)
{
	binn		*results;
	uint32_t	res_cnt;

	binn_object_get_uint32(obj, "byte_size", &set->byte_size);
	binn_object_get_uint32(obj, "result_cnt", &res_cnt);
	set->result_cnt += res_cnt;
	if (binn_object_get_list(obj, "hstgrp", (void **)&results) == true)
		get_resultlist_from_binnlist(results, &set->results, work);
}

void			unbinnify_targetset(t_targetset *set, binn *obj)
{
	binn		*ip4list;
	binn		*ip4rnglist;

	binn_object_get_uint32(obj, "total", &set->total);
	binn_object_get_uint32(obj, "ip_cnt", &set->ip_cnt);
	binn_object_get_uint32(obj, "rng_cnt", &set->rng_cnt);
	if (binn_object_get_list(obj, "ips", (void **)&ip4list) == true)
		get_ip4list_from_binnlist(&set->ips, ip4list);
	if (binn_object_get_list(obj, "iprngs", (void**)&ip4rnglist) == true)
		get_ip4rnglist_from_binnlist(&set->iprngs, ip4rnglist);
}

void			unbinnify_portset(t_portset *set, binn *obj)
{
	binn		*portlist;
	binn		*prtrnglist;

	binn_object_get_uint16(obj, "total", &set->total);
	binn_object_get_uint16(obj, "port_cnt", &set->port_cnt);
	binn_object_get_uint16(obj, "rng_cnt", &set->rng_cnt);
	if (binn_object_get_list(obj, "ports", (void **)&portlist) == true)
		get_portlist_from_binnlist(&set->ports, portlist);
	if (binn_object_get_list(obj, "prtrngs", (void**)&prtrnglist) == true)
		get_prtrnglist_from_binnlist(&set->prtrngs, prtrnglist);
}

void			unbinnify_opts(t_opts *opts, binn *obj)
{
	binn_object_get_uint64(obj,"bitops", (uint64 *)&opts->bitops);
	binn_object_get_uint32(obj, "min_rtt_timeout", &opts->min_rtt_timeo);
	binn_object_get_uint32(obj, "init_rtt_timeo", &opts->init_rtt_timeo);
	binn_object_get_uint32(obj, "max_rtt_timeo", &opts->max_rtt_timeo);
	binn_object_get_uint32(obj, "host_timeo", &opts->host_timeo);
	binn_object_get_uint32(obj, "scan_delay", &opts->scan_delay);
	binn_object_get_uint32(obj, "max_scan_delay", &opts->max_scan_delay);
	binn_object_get_uint32(obj, "min_packet_rate", &opts->min_packet_rate);
	binn_object_get_uint32(obj, "max_packet_rate", &opts->max_packet_rate);
	binn_object_get_uint32(obj, "max_retries", &opts->max_retries);
	binn_object_get_uint32(obj, "min_parallel", &opts->min_parallel);
	binn_object_get_uint32(obj, "max_parallel", &opts->max_parallel);
	binn_object_get_uint32(obj, "min_hostgroup", &opts->min_hostgroup);
	binn_object_get_uint32(obj, "max_hostgroup", &opts->max_hostgroup);
	binn_object_get_uint32(obj, "spoofed_srcaddr", &opts->spoofed_srcaddr);
	binn_object_get_uint16(obj, "spoofed_srcport", &opts->spoofed_srcport);
	binn_object_get_uint16(obj, "rpayload_len", &opts->rpayload_len);
	binn_object_get_uint8(obj, "fragment_mtu", &opts->fragment_mtu);
	binn_object_get_uint8(obj, "ip_ttl", &opts->ip_ttl);
	binn_object_get_uint8(obj, "thread_count", &opts->thread_count);
	binn_object_get_uint8(obj, "verbose_level", &opts->verbose_level);
}

/* TODO more error checking! */
void			unbinnify_env(t_env *env, binn *obj)
{
	binn		*member;

	binn_object_get_object(obj, "opts", (void **)&member);
	unbinnify_opts(&env->opts, member);
	binn_object_get_object(obj, "ports", (void**)&member);
	unbinnify_portset(&env->ports, member);
	if (binn_object_get_object(obj, "ack_ports", (void **)&member) == true)
	{
		env->ack_ports = new_portset();
		unbinnify_portset(env->ack_ports, member);
	}
	if (binn_object_get_object(obj, "syn_ports", (void **)&member) == true)
	{
		env->syn_ports = new_portset();
		unbinnify_portset(env->syn_ports, member);
	}
	if (binn_object_get_object(obj, "udp_ports", (void **)&member) == true)
	{
		env->udp_ports = new_portset();
		unbinnify_portset(env->udp_ports, member);
	}
}