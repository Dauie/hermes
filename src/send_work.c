#include "../incl/job.h"
#include "../binn/src/binn.h"

typedef enum s_msg_types
{
	OPS_MSG,
	TARGETS_MSG,
	PORT_MSG,
}			t_msg_types;


void			add_ip4_binnlist(binn **list, t_node *ips)
{
	if (ips->left)
		add_ip4_binnlist(list, ips->left);
	binn_list_add_uint32(*list, ((t_ip4 *)ips->data)->s_addr);
	if (ips->right)
		add_ip4_binnlist(list, ips->right);
}

void			add_ip4rng_binnlist(binn **list, t_node *ip4rng)
{
	t_ip4rng	*rng;
	binn		*obj;

	if (ip4rng->left)
		add_ip4rng_binnlist(list, ip4rng->left);
	rng = ip4rng->data;
	obj = binn_object();
	binn_object_set_uint32(obj, "size", rng->size);
	binn_object_set_uint32(obj, "start", rng->start);
	binn_object_set_uint32(obj, "end", rng->end);
	binn_list_add_object(*list, obj);
	if (ip4rng->right)
		add_ip4rng_binnlist(list, ip4rng->right);
}

void				add_port_binnlist(binn **list, t_node *ports)
{
	if (ports->left)
		add_port_binnlist(list, ports->left);
	binn_list_add_uint16(*list, ((t_port *)ports->data)->port);
	if (ports->right)
		add_port_binnlist(list, ports->right);
}

void				add_prtrng_binnlist(binn **list, t_node *prtrngs)
{
	t_prtrng	*rng;
	binn		*obj;

	if (prtrngs->left)
		add_prtrng_binnlist(list, prtrngs->left);
	rng = prtrngs->data;
	obj = binn_object();
	binn_object_set_uint32(obj, "size", rng->size);
	binn_object_set_uint32(obj, "start", rng->start);
	binn_object_set_uint32(obj, "end", rng->end);
	binn_list_add_object(*list, obj);
	if (prtrngs->right)
		add_prtrng_binnlist(list, prtrngs->right);
}

binn				*make_targetlist_binn(t_targetlist *targets)
{
	binn			*obj;
	binn			*ip;
	binn			*iprng;

	obj = binn_object();
	ip = binn_list();
	iprng = binn_list();
	binn_object_set_uint8(obj, "msg_type", TARGETS_MSG);
	binn_object_set_uint32(obj, "total", targets->total);
	binn_object_set_uint32(obj, "ip_cnt", targets->ip_cnt);
	binn_object_set_uint32(obj, "rng_cnt", targets->rng_cnt);
	add_ip4_binnlist(&ip, targets->ips);
	add_ip4rng_binnlist(&iprng, targets->iprngs);
	binn_object_set_list(obj, "ips", ip);
	binn_object_set_list(obj, "iprngs", iprng);
	return (obj);
}

binn				*make_portlist_binn(t_portlist *ports, uint8_t msgtype)
{
	binn			*obj;
	binn			*port;
	binn			*prtrng;

	obj = binn_object();
	port = binn_list();
	prtrng = binn_list();
	binn_object_set_uint8(obj, "msg_type", msgtype);
	binn_object_set_uint16(obj, "total", ports->total);
	binn_object_set_uint16(obj, "port_cnt", ports->port_cnt);
	binn_object_set_uint16(obj, "rng_cnt", ports->rng_cnt);
	binn_object_set_list(obj, "ports", port);
	binn_object_set_list(obj, "prtrngs", prtrng);
	return (obj);
}

binn				*make_ops_binn(t_ops *options)
{
	binn			*ops;
	uint64_t		*bitops;

	if (!(ops = binn_object()))
		hermes_error(FAILURE, TRUE, 1, "binn_objet() failed");
	binn_object_set_uint8(ops, "msg_type", OPS_MSG);
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

int				send_work(t_worker *worker, t_job *job)
{


	binn			*ports;
	binn			*ops;
	binn			*targets;

	ops = make_ops_binn(&job->options);
	targets = make_targetlist_binn(&job->targets);
	ports = make_portlist_binn(&job->ports, PORT_MSG);

	printf("ops size: %d\n", binn_size(ops));
	printf("targets size: %d\n",binn_size(targets));
	printf("ports size: %d\n",binn_size(ports));
	(void)worker;
	return (SUCCESS);
}