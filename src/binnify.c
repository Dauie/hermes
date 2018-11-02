#include "../incl/hermes.h"

static void		add_ip4list_to_binnlist(binn *list, t_node **ips)
{
	t_node		*head;

	head = *ips;
	while (head)
	{
		binn_list_add_uint32(list, ((t_ip4 *)head->data)->s_addr);
		head = head->right;
	}
}

static void		add_ip4rnglist_to_binnlist(binn *list, t_node **ip4rng)
{
	t_node		*head;
	t_ip4rng	*rng;
	binn		*obj;

	head = *ip4rng;
	while (head)
	{
		rng = head->data;
		obj = binn_object();
		binn_object_set_uint32(obj, "size", rng->size);
		binn_object_set_uint32(obj, "start", rng->start);
		binn_object_set_uint32(obj, "end", rng->end);
		binn_list_add_object(list, obj);
		head = head->right;
	}
}

static void		add_portlist_to_binnlist(binn *list, t_node **ports)
{
	t_node		*head;
	t_port		*port;

	head = *ports;
	while (head)
	{
		port = head->data;
		binn_list_add_uint16(list, port->port);
		head = head->right;
	}
}

static void		add_prtrnglist_to_binnlist(binn *list, t_node **prtrngs)
{
	t_node		*head;
	t_prtrng	*rng;
	binn		*obj;

	head = *prtrngs;
	while (head)
	{
		rng = head->data;
		obj = binn_object();
		binn_object_set_uint32(obj, "size", rng->size);
		binn_object_set_uint32(obj, "start", rng->start);
		binn_object_set_uint32(obj, "end", rng->end);
		binn_list_add_object(list, obj);
		head = head->right;
	}
}

static void		add_portstatlist_to_binnlist(binn *list, t_node **port_stats)
{
	t_node		*head;
	t_portstat	*port_stat;
	binn		*obj;

	head = *port_stats;
	while (head)
	{
		port_stat = head->data;
		obj = binn_object();
		binn_object_set_uint16(obj, "port", port_stat->port);
		binn_object_set_uint8(obj, "status", port_stat->status);
		binn_list_add_object(list, obj);
		binn_free(obj);
		head = head->right;
	}
}

static void		add_resultlist_to_binnlist(binn *list, t_node **results)
{
	t_node		*head;
	t_result	*result;
	binn		*res_binn;
	binn		*portstatlist;

	head = *results;
	while (head)
	{
		result = head->data;
		res_binn = binn_object();
		binn_object_set_uint32(res_binn, "ip", result->ip.s_addr);
		if (result->port_stats)
		{
			portstatlist = binn_list();
			add_portstatlist_to_binnlist(portstatlist, &result->port_stats);
			binn_object_set_list(res_binn, "port_stats", portstatlist);
			binn_free(portstatlist);
			del_list(&result->port_stats, true);
		}
		else
			binn_object_set_null(res_binn, "port_stats");
		binn_list_add_object(list, res_binn);
		binn_free(res_binn);
		head = head->right;
	}
}

binn			*binnify_resultset(t_resultset *set)
{
	binn		*obj;
	binn		*result_list;

	if (!(obj = binn_object()))
	{
		hermes_error(FAILURE, "binn_object %s", strerror(errno));
		return (NULL);
	}
	binn_object_set_uint32(obj, "byte_size", set->byte_size);
	binn_object_set_uint32(obj, "result_cnt", set->result_cnt);
	if (set->results)
	{
		result_list = binn_list();
		add_resultlist_to_binnlist(result_list, &set->results);
		binn_object_set_list(obj, "results", result_list);
		binn_free(result_list);
		del_list(&set->results, true);
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
	printf("binnifying targets total: %d ip_cnt: %d  rng_cnt: %d\n", set->total, set->ip_cnt, set->rng_cnt);
	binn_object_set_uint32(obj, "total", set->total);
	binn_object_set_uint32(obj, "ip_cnt", set->ip_cnt);
	binn_object_set_uint32(obj, "rng_cnt", set->rng_cnt);
	if (set->ips)
	{
		ip = binn_list();
		add_ip4list_to_binnlist(ip, &set->ips);
		binn_object_set_list(obj, "ips", ip);
		binn_free(ip);
	}
	else
		binn_object_set_null(obj, "ips");
	if (set->iprngs)
	{
		iprng = binn_list();
		add_ip4rnglist_to_binnlist(iprng, &set->iprngs);
		binn_object_set_list(obj, "iprngs", iprng);
		binn_free(iprng);
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
		add_portlist_to_binnlist(port, &set->ports);
		binn_object_set_list(obj, "ports", port);
		binn_free(port);

	}
	else
		binn_object_set_null(obj, "ports");
	if (set->prtrngs)
	{
		if (!(prtrng = binn_list()))
		{
			hermes_error(FAILURE, "binn_list() failed");
			return (NULL);
		}
		add_prtrnglist_to_binnlist(prtrng, &set->prtrngs);
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

	if (!(obj = binn_object()))
	{
		hermes_error(FAILURE, "binn_object()");
		return (NULL);
	}
	member = binnify_opts(&env->opts);
	binn_object_set_object(obj, "opts", member);
	binn_free(member);
	member = binnify_portset(&env->ports);
	binn_object_set_object(obj, "ports", member);
	binn_free(member);
	if (env->ack_ports) {
		member = binnify_portset(env->ack_ports);
		binn_object_set_object(obj, "ack_ports", member);
		binn_free(member);
	}
	if (env->syn_ports) {
		member = binnify_portset(env->syn_ports);
		binn_object_set_object(obj, "syn_ports", member);
		binn_free(member);
	}
	if (env->udp_ports)
	{
		member = binnify_portset(env->udp_ports);
		binn_object_set_object(obj, "udp_ports", member);
		binn_free(member);
	}
	return (obj);
}
