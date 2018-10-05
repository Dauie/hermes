#include <hermes.h>
#include "../binn/src/binn.h"
#include "../incl/job.h"

void			add_ip4tree_to_binnlist(binn **list, t_node *ips)
{
	if (ips->left)
		add_ip4tree_to_binnlist(list, ips->left);

	binn_list_add_uint32(*list, ((t_ip4 *)ips->data)->s_addr);

	if (ips->right)
		add_ip4tree_to_binnlist(list, ips->right);
}

void			add_ip4rngtree_to_binnlist(binn **list, t_node *ip4rng)
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

void				add_porttree_to_binnlist(binn **list, t_node *ports)
{
	if (ports->left)
		add_porttree_to_binnlist(list, ports->left);

	binn_list_add_uint16(*list, ((t_port *)ports->data)->port);

	if (ports->right)
		add_porttree_to_binnlist(list, ports->right);
}

void				add_prtrngtree_to_binnlist(binn **list, t_node *prtrngs)
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

binn				*make_targetlist_binn(t_targetlist *list)
{
	binn			*obj;
	binn			*ip;
	binn			*iprng;

	obj = binn_object();
	ip = binn_list();
	iprng = binn_list();

	binn_object_set_uint32(obj, "total", list->total);
	binn_object_set_uint32(obj, "ip_cnt", list->ip_cnt);
	binn_object_set_uint32(obj, "rng_cnt", list->rng_cnt);

	add_ip4tree_to_binnlist(&ip, list->ips);
	add_ip4rngtree_to_binnlist(&iprng, list->iprngs);

	binn_object_set_list(obj, "ips", ip);
	binn_object_set_list(obj, "iprngs", iprng);
	return (obj);
}

binn				*make_portlist_binn(t_portlist *list)
{
	binn			*obj;
	binn			*port;
	binn			*prtrng;

	obj = binn_object();
	port = binn_list();
	prtrng = binn_list();

	binn_object_set_uint16(obj, "total", list->total);
	binn_object_set_uint16(obj, "port_cnt", list->port_cnt);
	binn_object_set_uint16(obj, "rng_cnt", list->rng_cnt);

	add_porttree_to_binnlist(&port, list->ports);
	add_prtrngtree_to_binnlist(&prtrng, list->prtrngs);

	binn_object_set_list(obj, "ports", port);
	binn_object_set_list(obj, "prtrngs", prtrng);

	binn_free(port);
	binn_free(prtrng);
	return (obj);
}

binn				*make_ops_binn(t_ops *options)
{
	binn			*ops;
	uint64_t		*bitops;

	if (!(ops = binn_object()))
		hermes_error(FAILURE, TRUE, 1, "binn_object() failed");
	bitops = (uint64_t*)&options->bitops;
	binn_object_set_uint64(ops, "bitops", *bitops);
	binn_object_set_uint32(ops, "min_rtt_timeout", options->min_rtt_timeo);
	binn_object_set_uint32(ops, "init_rtt_timeo", options->init_rtt_timeo);
	binn_object_set_uint32(ops, "max_rtt_timeo", options->max_rtt_timeo);
	binn_object_set_uint32(ops, "host_timeo", options->host_timeo);
	binn_object_set_uint32(ops, "scan_delay", options->scan_delay);
	binn_object_set_uint32(ops, "max_scan_delay", options->max_scan_delay);
	binn_object_set_uint32(ops, "min_packet_rate", options->min_packet_rate);
	binn_object_set_uint32(ops, "max_packet_rate", options->max_packet_rate);
	binn_object_set_uint32(ops, "max_retries", options->max_retries);
	binn_object_set_uint32(ops, "min_parallel", options->min_parallel);
	binn_object_set_uint32(ops, "max_parallel", options->max_parallel);
	binn_object_set_uint32(ops, "min_hostgroup", options->min_hostgroup);
	binn_object_set_uint32(ops, "max_hostgroup", options->max_hostgroup);
	binn_object_set_uint32(ops, "spoofed_srcaddr", options->spoofed_srcaddr);
	binn_object_set_uint16(ops, "spoofed_srcport", options->spoofed_srcport);
	binn_object_set_uint16(ops, "rpayload_len", options->rpayload_len);
	binn_object_set_uint8(ops, "fragment_mtu", options->fragment_mtu);
	binn_object_set_uint8(ops, "ip_ttl", options->ip_ttl);
	binn_object_set_uint8(ops, "thread_count", options->thread_count);
	binn_object_set_uint8(ops, "verbose_level", options->verbose_level);
	return (ops);
}

void				get_ops_from_binn(t_ops *ops, binn *obj)
{
	binn_object_get_uint64(obj,"bitops", (uint64 *)&ops->bitops);
	binn_object_get_uint32(ops, "min_rtt_timeout", &ops->min_rtt_timeo);
	binn_object_get_uint32(ops, "init_rtt_timeo", &ops->init_rtt_timeo);
	binn_object_get_uint32(ops, "max_rtt_timeo", &ops->max_rtt_timeo);
	binn_object_get_uint32(ops, "host_timeo", &ops->host_timeo);
	binn_object_get_uint32(ops, "scan_delay", &ops->scan_delay);
	binn_object_get_uint32(ops, "max_scan_delay", &ops->max_scan_delay);
	binn_object_get_uint32(ops, "min_packet_rate", &ops->min_packet_rate);
	binn_object_get_uint32(ops, "max_packet_rate", &ops->max_packet_rate);
	binn_object_get_uint32(ops, "max_retries", &ops->max_retries);
	binn_object_get_uint32(ops, "min_parallel", &ops->min_parallel);
	binn_object_get_uint32(ops, "max_parallel", &ops->max_parallel);
	binn_object_get_uint32(ops, "min_hostgroup", &ops->min_hostgroup);
	binn_object_get_uint32(ops, "max_hostgroup", &ops->max_hostgroup);
	binn_object_get_uint32(ops, "spoofed_srcaddr", &ops->spoofed_srcaddr);
	binn_object_get_uint16(ops, "spoofed_srcport", &ops->spoofed_srcport);
	binn_object_get_uint16(ops, "rpayload_len", &ops->rpayload_len);
	binn_object_get_uint8(ops, "fragment_mtu", &ops->fragment_mtu);
	binn_object_get_uint8(ops, "ip_ttl", &ops->ip_ttl);
	binn_object_get_uint8(ops, "thread_count", &ops->thread_count);
	binn_object_get_uint8(ops, "verbose_level", &ops->verbose_level);
}

static void				get_porttree_from_binnlist(t_node **tree, binn *list)
{
	int				i;
	int				cnt;
	t_port			*port;

	i = 0;
	cnt = binn_count(list);
	while (++i <= cnt)
	{
		port = new_port();
		port->port = binn_list_uint16(list, i);
		add_node_bst(tree, (void **)&port, port_cmp);
	}
}

static void				get_ip4tree_from_binnlist(t_node **tree, binn *list)
{
	int				i;
	int				cnt;
	t_ip4			*ip;

	i = 0;
	cnt = binn_count(list);
	while (++i <= cnt)
	{
		ip = new_ip4();
		ip->s_addr = binn_list_uint32(list, i);
		add_node_bst(tree, (void **)&ip, ip4_cmp);
	}
}


static void				get_prtrngtree_from_binnlist(t_node **tree, binn *list)
{
	int				i;
	int				cnt;
	t_prtrng		*rng;
	binn			*obj;

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

static void				get_ip4rngtree_from_binnlist(t_node **tree, binn *list)
{
	int				i;
	int				cnt;
	t_ip4rng		*rng;
	binn			*obj;

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

void				get_portlist_from_binn(t_portlist *list, binn *obj)
{
	binn			*portlist;
	binn			*prtrnglist;

	binn_object_get_uint16(obj, "total", &list->total);
	binn_object_get_uint16(obj, "port_cnt", &list->port_cnt);
	binn_object_get_uint16(obj, "rng_cnt", &list->rng_cnt);

	/* TODO This may not work */

	binn_object_get_list(obj, "ports", (void **)&portlist);
	binn_object_get_list(obj, "prtrngs", (void**)&prtrnglist);

	get_porttree_from_binnlist(&list->ports, portlist);
	get_prtrngtree_from_binnlist(&list->prtrngs, prtrnglist);
}

void				get_targetlist_from_binn(t_targetlist *list, binn *obj)
{
	binn			*ip4list;
	binn			*ip4rnglist;

	binn_object_get_uint32(obj, "prtrngtotal", &list->total);
	binn_object_get_uint32(obj, "ip_cnt", &list->ip_cnt);
	binn_object_get_uint32(obj, "rng_cnt", &list->rng_cnt);

	/* TODO This may not work */

	binn_object_get_list(obj, "ips", (void **)&ip4list);
	binn_object_get_list(obj, "iprngs", (void**)&ip4rnglist);

	get_ip4tree_from_binnlist(&list->ips, ip4list);
	get_ip4rngtree_from_binnlist(&list->iprngs, ip4rnglist);
}

