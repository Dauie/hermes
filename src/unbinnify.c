#include "../incl/hermes.h"

static void		get_portclist_from_binnlist(t_node **tree, binn *list)
{
	int			i;
	int			cnt;
	t_port		*port;

	i = 0;
	cnt = binn_count(list);
	while (i++ <= cnt)
	{
		port = new_port();
		port->port = binn_list_uint16(list, i);
		clist_add_head(tree, (void **)&port);
	}
}

static void		get_ip4clist_from_binnlist(t_node **clist, binn *list)
{
	int			i;
	int			cnt;
	t_ip4		*ip;

	i = 0;
	cnt = binn_count(list);
	while (i++ <= cnt)
	{
		ip = new_ip4();
		ip->s_addr = binn_list_uint32(list, i);
		clist_add_head(clist, (void **)&ip);
	}
}

static void		get_prtrngclist_from_binnlist(t_node **clst, binn *list)
{
	int			i;
	int			cnt;
	t_prtrng	*rng;
	binn		*obj;

	i = 1;
	cnt = binn_count(list);
	while (i++ <= cnt)
	{
		rng = new_portrange();
		obj = binn_list_object(list, i);
		binn_object_get_uint16(obj, "size", &rng->size);
		binn_object_get_uint16(obj, "start", &rng->start);
		binn_object_get_uint16(obj, "end", &rng->end);
		clist_add_head(clst, (void **)&rng);
	}
}

static void		get_ip4rngclist_from_binnlist(t_node **clst, binn *list)
{
	int			i;
	int			cnt;
	t_ip4rng	*rng;
	binn		*obj;

	i = 0;
	cnt = binn_count(list);
	while (i++ <= cnt)
	{
		rng = new_ip4range();
		obj = binn_list_object(list, i);
		binn_object_get_uint32(obj, "size", &rng->size);
		binn_object_get_uint32(obj, "start", &rng->start);
		binn_object_get_uint32(obj, "end", &rng->end);
		clist_add_head(clst, (void **)&rng);
	}
}

void			get_portstatclist_from_binnlist(binn *list, t_node **clist)
{
	int			i;
	int			cnt;
	t_portstat	*pstat;
	binn		*obj;

	i = 0;
	cnt = binn_count(list);
	while (i++ < cnt)
	{
		pstat = new_portstat();
		obj = binn_list_object(list, i);
		binn_object_get_uint16(obj, "port", &pstat->port);
		binn_object_get_uint8(obj, "status", &pstat->status);
		clist_add_head(clist, (void **)&pstat);
	}
}

void			get_resultclist_from_binnlist(binn *list, t_node **clist, t_targetset *account)
{
	int			i;
	int			cnt;
	t_result	*res;
	binn		*obj;
	binn		*portstats;

	i = 0;
	cnt = binn_count(list);
	printf("count %d\n", cnt);
	while (i++ <= cnt)
	{
		printf("%d\n", i);
		res = new_result();
		obj = binn_list_object(list, i);
		binn_object_get_uint32(obj, "ip", &res->ip.s_addr);
		remove_ip_targetset(account, res->ip.s_addr);
		if (binn_object_get_object(obj, "port_stats", (void **)&portstats) == true)
		{
			get_portstatclist_from_binnlist(portstats, &res->port_stats);
		}
		clist_add_head(clist, (void **)&res);
	}
}

void			unbinnify_resultset(t_resultset *set, t_targetset *work,  binn *obj)
{
	binn		*results;
	uint32_t	res_cnt;

	binn_object_get_uint32(obj, "byte_size", &set->byte_size);
	binn_object_get_uint32(obj, "result_cnt", &res_cnt);
	set->result_cnt += res_cnt;
	if (binn_object_get_list(obj, "results", (void **)&results) == true)
	{
		get_resultclist_from_binnlist(results, &set->results, work);
	}
}

void			unbinnify_targetset(t_targetset *set, binn *obj)
{
	binn		*ip4list;
	binn		*ip4rnglist;

	binn_object_get_uint32(obj, "total", &set->total);
	binn_object_get_uint32(obj, "ip_cnt", &set->ip_cnt);
	binn_object_get_uint32(obj, "rng_cnt", &set->rng_cnt);
	if (binn_object_get_list(obj, "ips", (void **)&ip4list) == true)
		get_ip4clist_from_binnlist(&set->ips, ip4list);
	if (binn_object_get_list(obj, "iprngs", (void**)&ip4rnglist) == true)
		get_ip4rngclist_from_binnlist(&set->iprngs, ip4rnglist);
}

void			unbinnify_portset(t_portset *set, binn *obj)
{
	binn		*portlist;
	binn		*prtrnglist;

	binn_object_get_uint16(obj, "total", &set->total);
	binn_object_get_uint16(obj, "port_cnt", &set->port_cnt);
	binn_object_get_uint16(obj, "rng_cnt", &set->rng_cnt);
	if (binn_object_get_list(obj, "port_stats", (void **)&portlist) == true)
		get_portclist_from_binnlist(&set->ports, portlist);
	if (binn_object_get_list(obj, "prtrngs", (void**)&prtrnglist) == true)
		get_prtrngclist_from_binnlist(&set->prtrngs, prtrnglist);
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

void			unbinnify_env(t_env *env, binn *obj)
{
	binn		*member;

	binn_object_get_object(obj, "opts", (void **)&member);
	unbinnify_opts(&env->opts, member);
	binn_object_get_object(obj, "port_stats", (void**)&member);
	unbinnify_portset(&env->ports, member);
	if (binn_object_get_object(obj, "ack_ports", (void **)&member) == true)
	{
		env->ack_ports = new_portset();
		unbinnify_portset(env->ack_ports, member);
		free(member);
	}
	if (binn_object_get_object(obj, "syn_ports", (void **)&member) == true)
	{
		env->syn_ports = new_portset();
		unbinnify_portset(env->syn_ports, member);
		free(member);
	}
	if (binn_object_get_object(obj, "udp_ports", (void **)&member) == true)
	{
		env->udp_ports = new_portset();
		unbinnify_portset(env->udp_ports, member);
		free(member);
	}
}