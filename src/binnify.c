#include "../incl/hermes.h"

static void		get_porttree_from_binnlist(t_node **tree, binn *list)
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
		add_node_bst(tree, (void **)&port, port_cmp);
	}
}

static void		get_ip4tree_from_binnlist(t_node **tree, binn *list)
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
		add_node_bst(tree, (void **)&ip, ip4_cmp);
	}
}

static void		get_prtrngtree_from_binnlist(t_node **tree, binn *list)
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
		add_node_bst(tree, (void **)&rng, port_cmp);
	}
}

static void		get_ip4rngtree_from_binnlist(t_node **tree, binn *list)
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
		add_node_bst(tree, (void **)&rng, port_cmp);
	}
}

static void		add_ip4tree_to_binnlist(binn **list, t_node *ips)
{
	if (ips->left)
		add_ip4tree_to_binnlist(list, ips->left);

	binn_list_add_uint32(*list, ((t_ip4 *)ips->data)->s_addr);

	if (ips->right)
		add_ip4tree_to_binnlist(list, ips->right);
}

static void		add_ip4rngtree_to_binnlist(binn **list, t_node *ip4rng)
{
	t_ip4rng	*rng;
	binn		*obj;

	if (ip4rng->left)
		add_ip4rngtree_to_binnlist(list, ip4rng->left);

	rng = ip4rng->data;
	obj = binn_object();

	binn_object_set_uint32(obj, "size", rng->size);
	binn_object_set_uint32(obj, "start", rng->start);
	binn_object_set_uint32(obj, "end", rng->end);

	binn_list_add_object(*list, obj);

	if (ip4rng->right)
		add_ip4rngtree_to_binnlist(list, ip4rng->right);
}

static void		add_porttree_to_binnlist(binn **list, t_node *ports)
{
	if (ports->left)
		add_porttree_to_binnlist(list, ports->left);

	binn_list_add_uint16(*list, ((t_port *)ports->data)->port);

	if (ports->right)
		add_porttree_to_binnlist(list, ports->right);
}

static void		add_prtrngtree_to_binnlist(binn **list, t_node *prtrngs)
{
	t_prtrng	*rng;
	binn		*obj;

	if (prtrngs->left)
		add_prtrngtree_to_binnlist(list, prtrngs->left);

	rng = prtrngs->data;
	obj = binn_object();

	binn_object_set_uint32(obj, "size", rng->size);
	binn_object_set_uint32(obj, "start", rng->start);
	binn_object_set_uint32(obj, "end", rng->end);

	binn_list_add_object(*list, obj);

	if (prtrngs->right)
		add_prtrngtree_to_binnlist(list, prtrngs->right);
}

binn			*binnify_opts(t_opts *opts)
{
	binn		*obj;
	uint64_t	*bitops;

	if (! opts)
		return (NULL);
	if (!(obj = binn_object()))
	{
		hermes_error(FAILURE, 1, "binn_object() failed");
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
	// TODO -> heap overflow
//	binn_object_set_uint32(obj, "min_packet_rate", opts->min_packet_rate);
//	binn_object_set_uint32(obj, "max_packet_rate", opts->max_packet_rate);
//	binn_object_set_uint32(obj, "max_retries", opts->max_retries);
//	binn_object_set_uint32(obj, "min_parallel", opts->min_parallel);
//	binn_object_set_uint32(obj, "max_parallel", opts->max_parallel);
//	binn_object_set_uint32(obj, "min_hostgroup", opts->min_hostgroup);
//	binn_object_set_uint32(obj, "max_hostgroup", opts->max_hostgroup);
//	binn_object_set_uint32(obj, "spoofed_srcaddr", opts->spoofed_srcaddr);
//	binn_object_set_uint16(obj, "spoofed_srcport", opts->spoofed_srcport);
//	binn_object_set_uint16(obj, "rpayload_len", opts->rpayload_len);
//	binn_object_set_uint8(obj, "fragment_mtu", opts->fragment_mtu);
//	binn_object_set_uint8(obj, "ip_ttl", opts->ip_ttl);
//	binn_object_set_uint8(obj, "thread_count", opts->thread_count);
//	binn_object_set_uint8(obj, "verbose_level", opts->verbose_level);
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
		hermes_error(FAILURE, 1, "binn_object() failed");
		return (NULL);
	}
	if (!(port = binn_list()))
	{
		hermes_error(FAILURE, 1, "binn_list() failed");
		return (NULL);
	}
	if (!(prtrng = binn_list()))
	{
		hermes_error(FAILURE, 1, "binn_list() failed");
		return (NULL);
	}
	binn_object_set_uint16(obj, "total", set->total);
	binn_object_set_uint16(obj, "port_cnt", set->port_cnt);
	binn_object_set_uint16(obj, "rng_cnt", set->rng_cnt);

	add_porttree_to_binnlist(&port, set->ports);
	add_prtrngtree_to_binnlist(&prtrng, set->prtrngs);

	/* add lists to main object */
	binn_object_set_list(obj, "ports", port);
	binn_object_set_list(obj, "prtrngs", prtrng);

	/* delete lists that were added */
	binn_free(port);
	binn_free(prtrng);
	return (obj);
}

binn			*binnify_targetset(t_targetset *set)
{
	binn		*obj;
	binn		*ip;
	binn		*iprng;

	obj = binn_object();
	ip = binn_list();
	iprng = binn_list();

	binn_object_set_uint32(obj, "total", set->total);
	binn_object_set_uint32(obj, "ip_cnt", set->ip_cnt);
	binn_object_set_uint32(obj, "rng_cnt", set->rng_cnt);

	add_ip4tree_to_binnlist(&ip, set->ips);
	add_ip4rngtree_to_binnlist(&iprng, set->iprngs);

	binn_object_set_list(obj, "ips", ip);
	binn_object_set_list(obj, "iprngs", iprng);
	return (obj);
}


void			unbinnify_opts(t_opts *opts, binn *obj)
{
	binn_object_get_uint64(obj,"bitops", (uint64 *)&opts->bitops);
	binn_object_get_uint32(opts, "min_rtt_timeout", &opts->min_rtt_timeo);
	binn_object_get_uint32(opts, "init_rtt_timeo", &opts->init_rtt_timeo);
	binn_object_get_uint32(opts, "max_rtt_timeo", &opts->max_rtt_timeo);
	binn_object_get_uint32(opts, "host_timeo", &opts->host_timeo);
	binn_object_get_uint32(opts, "scan_delay", &opts->scan_delay);
	binn_object_get_uint32(opts, "max_scan_delay", &opts->max_scan_delay);
	binn_object_get_uint32(opts, "min_packet_rate", &opts->min_packet_rate);
	binn_object_get_uint32(opts, "max_packet_rate", &opts->max_packet_rate);
	binn_object_get_uint32(opts, "max_retries", &opts->max_retries);
	binn_object_get_uint32(opts, "min_parallel", &opts->min_parallel);
	binn_object_get_uint32(opts, "max_parallel", &opts->max_parallel);
	binn_object_get_uint32(opts, "min_hostgroup", &opts->min_hostgroup);
	binn_object_get_uint32(opts, "max_hostgroup", &opts->max_hostgroup);
	binn_object_get_uint32(opts, "spoofed_srcaddr", &opts->spoofed_srcaddr);
	binn_object_get_uint16(opts, "spoofed_srcport", &opts->spoofed_srcport);
	binn_object_get_uint16(opts, "rpayload_len", &opts->rpayload_len);
	binn_object_get_uint8(opts, "fragment_mtu", &opts->fragment_mtu);
	binn_object_get_uint8(opts, "ip_ttl", &opts->ip_ttl);
	binn_object_get_uint8(opts, "thread_count", &opts->thread_count);
	binn_object_get_uint8(opts, "verbose_level", &opts->verbose_level);
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

	get_porttree_from_binnlist(&set->ports, portlist);
	get_prtrngtree_from_binnlist(&set->prtrngs, prtrnglist);
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

	get_ip4tree_from_binnlist(&set->ips, ip4list);
	get_ip4rngtree_from_binnlist(&set->iprngs, ip4rnglist);
}
