#include "../incl/hermes.h"
#include "../incl/binnify.h"
#include "../incl/message.h"

void		connect_workers(t_node **workers, t_workerset *set, int proto)
{
	t_wrkr		*worker;

	if (!*workers)
		return ;
	if ((*workers)->left)
		connect_workers(&(*workers)->left, set, proto);
	worker = (t_wrkr*)(*workers)->data;
	if ((worker->sock = socket(PF_INET, SOCK_STREAM, proto)) == -1)
		hermes_error(EXIT_FAILURE, 2, "socket()", strerror(errno));
	if (connect(worker->sock, (const struct sockaddr *)&worker->sin, sizeof(worker->sin)) == -1)
	{
		hermes_error(FAILURE, 2, "could not connect to worker. dropping:", inet_ntoa(worker->sin.sin_addr));
		if (remove_node_bst(&set->wrkrs, worker, worker_cmp, worker_min) == true)
			set->cnt -= 1;
	}
	else
	{
		worker->stat.connected = true;
		printf("connected to %s.\n", inet_ntoa(worker->sin.sin_addr));
	}
	if ((*workers)->right)
		connect_workers(&(*workers)->right, set, proto);
}

void				send_workers_binn(t_node **workers, t_workerset *set, binn *obj, uint8_t code)
{
	t_wrkr			*wrkr;

	if ((*workers)->left)
		send_workers_binn(&(*workers)->left, set, obj, code);
	wrkr = (*workers)->data;
	hermes_send_binn(wrkr->sock, code, obj);
	if ((*workers)->right)
		send_workers_binn(&(*workers)->right, set, obj, code);
}

int					manager_loop(t_mgr *mgr)
{
	struct protoent	*proto;
	binn			*opts;
	binn			*ports;
	binn			*ack_ports;
	binn			*syn_ports;
	binn			*udp_ports;

	mgr->stat.running = true;
	if ((proto = getprotobyname("tcp")) == 0)
		return (FAILURE);
	if (mgr->workers && mgr->workers->cnt > 0)
	{
		connect_workers(&mgr->workers->wrkrs, mgr->workers, proto->p_proto);
		printf("connected to %i workers.\n", mgr->workers->cnt);
		opts = binnify_opts(&mgr->job.opts);
		send_workers_binn(&mgr->workers->wrkrs, mgr->workers, opts, C_OBJ_OPTS);
		free(opts);
		ports = binnify_portset(mgr->job.ports);
		send_workers_binn(&mgr->workers->wrkrs, mgr->workers, ports, C_OBJ_PS_NRM);
		free(ports);
		if (mgr->job.ack_ports)
		{
			ack_ports = binnify_portset(mgr->job.ack_ports);
			send_workers_binn(&mgr->workers->wrkrs, mgr->workers, ack_ports, C_OBJ_PS_ACK);
			free(ack_ports);
		}
		if (mgr->job.syn_ports)
		{
			syn_ports = binnify_portset(mgr->job.syn_ports);
			send_workers_binn(&mgr->workers->wrkrs, mgr->workers, syn_ports, C_OBJ_PS_SYN);
			free(syn_ports);
		}
		if (mgr->job.udp_ports)
		{
			udp_ports = binnify_portset(mgr->job.udp_ports);
			send_workers_binn(&mgr->workers->wrkrs, mgr->workers, udp_ports, C_OBJ_PS_ACK);
			free(udp_ports);
		}
	}
	/* TODO Spawn thread pool */
	while (mgr->stat.running == true)
	{
		/* TODO grab "normal" chunk of ips from mgr->job and assign them to mgr->workers, and mgr->cwork */
		/* TODO split mgr->cwork into smaller target_sets and assign to targetset_list for threads */
		/* TODO pass divided work to threads */
		if (mgr->job.targets->total == 0)
		{
			if (mgr->workers && mgr->workers->wrking_cnt > 0)
				;
			else
				mgr->stat.running = false;
		}
//		else if (mgr->job.targets->total > 0 && !mgr->cwork)
//		{
//			mgr->cwork =
//		}
	}
	return (0);
}

