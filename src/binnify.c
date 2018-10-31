#include "../incl/hermes.h"


static void		add_ip4clist_to_binnlist(binn *list, t_node **ips)
{
	t_node		*head;

	head = *ips;
	do {
		binn_list_add_uint32(list, ((t_ip4 *)head->data)->s_addr);
		head = head->right;
	} while (head != *ips);
}

static void		add_ip4rngclist_to_binnlist(binn *list, t_node **ip4rng)
{
	t_node		*head;
	t_ip4rng	*rng;
	binn		*obj;

	head = *ip4rng;
	do {
		rng = head->data;
		obj = binn_object();
		binn_object_set_uint32(obj, "size", rng->size);
		binn_object_set_uint32(obj, "start", rng->start);
		binn_object_set_uint32(obj, "end", rng->end);
		binn_list_add_object(list, obj);
		head = head->right;
	} while (head != *ip4rng);
}

static void		add_portclist_to_binnlist(binn *list, t_node **ports)
{
	t_node		*head;
	t_port		*port;

	head = *ports;
	do {
		port = head->data;
		binn_list_add_uint16(list, port->port);
		head = head->right;
	} while (head != *ports);
}

static void		add_prtrngclist_to_binnlist(binn *list, t_node **prtrngs)
{
	t_node		*head;
	t_prtrng	*rng;
	binn		*obj;

	head = *prtrngs;
	do {
		rng = head->data;
		obj = binn_object();
		binn_object_set_uint32(obj, "size", rng->size);
		binn_object_set_uint32(obj, "start", rng->start);
		binn_object_set_uint32(obj, "end", rng->end);
		binn_list_add_object(list, obj);
		head = head->right;
	} while (head != *prtrngs);
}

static void		add_portstatclist_to_binnlist(binn *list, t_node **portstatus)
{
	t_node		*head;
	t_portstat *port_stat;
	binn		*obj;

	head = *portstatus;
	do {
		port_stat = head->data;
		obj = binn_object();
		binn_object_set_uint16(obj, "port", port_stat->port);
		binn_object_set_uint8(obj, "status", port_stat->status);
		binn_list_add_object(list, obj);
		free(obj);
		head = head->right;
	} while (head != *portstatus);
}

static void		add_resultclist_to_binnlist(binn *list, t_node **results)
{
	t_node		*head;
	t_result	*res;
	binn		*obj;
	binn		*portstatlist;

	head = *results;
	if (!head->data)
		return ;
	do {
		res = head->data;
		obj = binn_object();
		binn_object_set_uint32(obj, "ip", res->ip.s_addr);
		if (res->port_stats)
		{
			portstatlist = binn_list();
			add_portstatclist_to_binnlist(portstatlist, &res->port_stats);
			binn_object_set_list(obj, "port_stats", portstatlist);
			free(portstatlist);
			del_clist(&res->port_stats, true);
		}
		else
			binn_object_set_null(obj, "port_stats");
		binn_list_add_object(list, obj);
		free(obj);
		head = head->right;
	} while (head != *results);
}

binn			*binnify_resultset(t_resultset *set)
{
	binn		*obj;
	binn		*res_list;

	obj = binn_object();
	binn_object_set_uint32(obj, "byte_size", set->byte_size);
	binn_object_set_uint32(obj, "result_cnt", set->result_cnt);
	if (set->results)
	{
		res_list = binn_list();
		add_resultclist_to_binnlist(res_list, &set->results);
		binn_object_set_list(obj, "results", res_list);
		free(res_list);
		del_clist(&set->results, true);
		set->result_cnt = 0;
		set->byte_size = 0;
	}
	else
		binn_object_set_null(obj, "results");
	return (obj);
}

binn			*binnify_targetset(t_targetset *set)
{
	binn		*obj;
	binn		*ip;
	binn		*iprng;

	if (!(obj = binn_object()))
	{
		hermes_error(FAILURE, "binn_object()");
		return (NULL);
	}
	binn_object_set_uint32(obj, "total", set->total);
	binn_object_set_uint32(obj, "ip_cnt", set->ip_cnt);
	binn_object_set_uint32(obj, "rng_cnt", set->rng_cnt);
	if (set->ips)
	{
		ip = binn_list();
		add_ip4clist_to_binnlist(ip, &set->ips);
		binn_object_set_list(obj, "ips", ip);
		free(ip);
	}
	else
		binn_object_set_null(obj, "ips");
	if (set->iprngs)
	{
		iprng = binn_list();
		add_ip4rngclist_to_binnlist(iprng, &set->iprngs);
		binn_object_set_list(obj, "iprngs", iprng);
		free(iprng);
	}
	else
		binn_object_set_null(obj, "iprngs");
	return (obj);
}

binn			*binnify_portset(t_portset *set)
{
	binn		*obj;
	binn		*port;
	binn		*prtrng;

	if (!set)
		return (NULL);
	if (!(obj = binn_object()))
	{
		hermes_error(FAILURE, "binn_object() failed");
		return (NULL);
	}
	binn_object_set_uint16(obj, "total", set->total);
	binn_object_set_uint16(obj, "port_cnt", set->port_cnt);
	binn_object_set_uint16(obj, "rng_cnt", set->rng_cnt);
	if (set->ports)
	{
		if (!(port = binn_list()))
		{
			hermes_error(FAILURE, "binn_list() failed");
			return (NULL);
		}
		add_portclist_to_binnlist(port, &set->ports);
		binn_object_set_list(obj, "port_stats", port);
		binn_free(port);

	}
	else
		binn_object_set_null(obj, "port_stats");
	if (set->prtrngs)
	{
		if (!(prtrng = binn_list()))
		{
			hermes_error(FAILURE, "binn_list() failed");
			return (NULL);
		}
		add_prtrngclist_to_binnlist(prtrng, &set->prtrngs);
		binn_object_set_list(obj, "prtrngs", prtrng);
		binn_free(prtrng);
	}
	else
		binn_object_set_null(obj, "prtrngs");
	return (obj);
}

binn			*binnify_opts(t_opts *opts)
{
	binn		*obj;
	uint64_t	*bitops;

	if (! opts)
		return (NULL);
	if (!(obj = binn_object()))
	{
		hermes_error(FAILURE, "binn_object() failed");
		return (NULL);
	}
	bitops = (uint64_t*)&opts->bitops;
	binn_object_set_uint64(obj, "bitops", *bitops);
	binn_object_set_uint32(obj, "min_rtt_timeout", opts->min_rtt_timeo);
	binn_object_set_uint32(obj, "init_rtt_timeo", opts->init_rtt_timeo);
	binn_object_set_uint32(obj, "max_rtt_timeo", opts->max_rtt_timeo);
	binn_object_set_uint32(obj, "host_timeo", opts->host_timeo);
	binn_object_set_uint32(obj, "scan_delay", opts->scan_delay);
	binn_object_set_uint32(obj, "max_scan_delay", opts->max_scan_delay);
	binn_object_set_uint32(obj, "min_packet_rate", opts->min_packet_rate);
	binn_object_set_uint32(obj, "max_packet_rate", opts->max_packet_rate);
	binn_object_set_uint32(obj, "max_retries", opts->max_retries);
	binn_object_set_uint32(obj, "min_parallel", opts->min_parallel);
	binn_object_set_uint32(obj, "max_parallel", opts->max_parallel);
	binn_object_set_uint32(obj, "min_hostgroup", opts->min_hostgroup);
	binn_object_set_uint32(obj, "max_hostmax_hostgroupgroup", opts->max_hostgroup);
	binn_object_set_uint32(obj, "spoofed_srcaddr", opts->spoofed_srcaddr);
	binn_object_set_uint16(obj, "spoofed_srcport", opts->spoofed_srcport);
	binn_object_set_uint16(obj, "rpayload_len", opts->rpayload_len);
	binn_object_set_uint8(obj, "fragment_mtu", opts->fragment_mtu);
	binn_object_set_uint8(obj, "ip_ttl", opts->ip_ttl);
	binn_object_set_uint8(obj, "thread_count", opts->thread_count);
	binn_object_set_uint8(obj, "verbose_level", opts->verbose_level);
	return (obj);
}

binn			*binnify_env(t_env *env)
{
	binn *obj;
	binn *member;

	if (!(obj = binn_object())) {
		hermes_error(FAILURE, "binn_object()");
		return (NULL);
	}
	member = binnify_opts(&env->opts);
	binn_object_set_object(obj, "opts", member);
	free(member);
	member = binnify_portset(&env->ports);
	binn_object_set_object(obj, "port_stats", member);
	free(member);
	if (env->ack_ports) {
		member = binnify_portset(env->ack_ports);
		binn_object_set_object(obj, "ack_ports", member);
		free(member);
	}
	if (env->syn_ports) {
		member = binnify_portset(env->syn_ports);
		binn_object_set_object(obj, "syn_ports", member);
		free(member);
	}
	if (env->udp_ports)
	{
		member = binnify_portset(env->udp_ports);
		binn_object_set_object(obj, "udp_ports", member);
		free(member);
	}
	return (obj);
}
