#include "../incl/hermes.h"

static void		get_portclist_from_binnlist(t_node **tree, binn *list)
{
	int			i;
	int			cnt;
	t_port		*port;

	i = 0;
	cnt = binn_count(list);
	while (++i <= cnt)
	{
		port = new_port();
		port->port = binn_list_uint16(list, i);
		clist_add_head(tree, (void **)&port);
	}
}

static void		get_ip4clist_from_binnlist(t_node **tree, binn *list)
{
	int			i;
	int			cnt;
	t_ip4		*ip;

	i = 0;
	cnt = binn_count(list);
	while (++i <= cnt)
	{
		ip = new_ip4();
		ip->s_addr = binn_list_uint32(list, i);
		clist_add_head(tree, (void **)&ip);
	}
}

static void		get_prtrngclist_from_binnlist(t_node **clst, binn *list)
{
	int			i;
	int			cnt;
	t_prtrng	*rng;
	binn		*obj;

	i = 0;
	cnt = binn_count(list);
	while (++i <= cnt)
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
	while (++i <= cnt)
	{
		rng = new_ip4range();
		obj = binn_list_object(list, i);
		binn_object_get_uint32(obj, "size", &rng->size);
		binn_object_get_uint32(obj, "start", &rng->start);
		binn_object_get_uint32(obj, "end", &rng->end);
		clist_add_head(clst, (void **)&rng);
	}
}

static void		add_ip4clist_to_binnlist(binn *list, t_node *ips)
{
	t_node		*head;

	head = ips;
	do {
		binn_list_add_uint32(list, ((t_ip4 *) ips->data)->s_addr);
		head = head->right;
	} while (head != ips);
}

static void		add_ip4rngclist_to_binnlist(binn *list, t_node *ip4rng)
{
	t_node		*head;
	t_ip4rng	*rng;
	binn		*obj;

	head = ip4rng;
	do {
		rng = ip4rng->data;
		obj = binn_object();

		binn_object_set_uint32(obj, "size", rng->size);
		binn_object_set_uint32(obj, "start", rng->start);
		binn_object_set_uint32(obj, "end", rng->end);

		binn_list_add_object(list, obj);
		head = head->right;
	} while (head != ip4rng);

}

static void		add_portclist_to_binnlist(binn *list, t_node *ports)
{
	t_node		*head;
	t_port		*port;

	head = ports;
	do {
		port = head->data;
		binn_list_add_uint16(list, port->port);
		head = head->right;
	} while (head != ports);
}

static void		add_prtrngclist_to_binnlist(binn *list, t_node *prtrngs)
{
	t_node		*head;
	t_prtrng	*rng;
	binn		*obj;

	head = prtrngs;
	do {
		rng = prtrngs->data;
		obj = binn_object();
		binn_object_set_uint32(obj, "size", rng->size);
		binn_object_set_uint32(obj, "start", rng->start);
		binn_object_set_uint32(obj, "end", rng->end);
		binn_list_add_object(list, obj);

	} while (head != prtrngs);

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

void			unbinnify_opts(t_opts *opts, binn *obj)
{
	printf("unbinnify ops started\n");
	printf("bin obj len %i\n", binn_size(obj));
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
	printf("leaving unbinnify ops\n");
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
		add_portclist_to_binnlist(port, set->ports);
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
		add_prtrngclist_to_binnlist(prtrng, set->prtrngs);
		binn_object_set_list(obj, "prtrngs", prtrng);
		binn_free(prtrng);
	}
	else
		binn_object_set_null(obj, "prtrngs");
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
		add_ip4clist_to_binnlist(ip, set->ips);
		binn_object_set_list(obj, "ips", ip);
		free(ip);
	}
	else
		binn_object_set_null(obj, "ips");
	if (set->iprngs)
	{
		iprng = binn_list();
		add_ip4rngclist_to_binnlist(iprng, set->iprngs);
		binn_object_set_list(obj, "iprngs", iprng);
		free(iprng);
	}
	else
		binn_object_set_null(obj, "iprngs");
	return (obj);
}

void			unbinnify_portset(t_portset *set, binn *obj)
{
	binn		*portlist;
	binn		*prtrnglist;

	binn_object_get_uint16(obj, "total", &set->total);
	binn_object_get_uint16(obj, "port_cnt", &set->port_cnt);
	binn_object_get_uint16(obj, "rng_cnt", &set->rng_cnt);


	binn_object_get_list(obj, "ports", (void **)&portlist);
	binn_object_get_list(obj, "prtrngs", (void**)&prtrnglist);

	get_portclist_from_binnlist(&set->ports, portlist);
	get_prtrngclist_from_binnlist(&set->prtrngs, prtrnglist);
}

void			unbinnify_targetset(t_targetset *set, binn *obj)
{
	binn		*ip4list;
	binn		*ip4rnglist;

	binn_object_get_uint32(obj, "total", &set->total);
	binn_object_get_uint32(obj, "ip_cnt", &set->ip_cnt);
	binn_object_get_uint32(obj, "rng_cnt", &set->rng_cnt);


	binn_object_get_list(obj, "ips", (void **)&ip4list);
	binn_object_get_list(obj, "iprngs", (void**)&ip4rnglist);

	get_ip4clist_from_binnlist(&set->ips, ip4list);
	get_ip4rngclist_from_binnlist(&set->iprngs, ip4rnglist);
}
