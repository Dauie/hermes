#include "../incl/hermes.h"
#include "../incl/binnify.h"
#include "../incl/message.h"

uint32_t			connect_workers(t_workerset *set, int proto)
{
	t_wrkr			*wrkr;
	uint32_t		iter;

	if (!set)
		return (0);
	iter = set->cnt;
	while (iter)
	{
		wrkr = (t_wrkr*)set->wrkrs->data;
		if ((wrkr->sock = socket(PF_INET, SOCK_STREAM, proto)) == -1)
			hermes_error(EXIT_FAILURE, "socket() %s", strerror(errno));
		if (connect(wrkr->sock, (const struct sockaddr *) &wrkr->sin,
					sizeof(wrkr->sin)) == -1)
		{
			hermes_error(FAILURE, "could not connect to worker. dropping: %s", inet_ntoa(wrkr->sin.sin_addr));
			set->wrkrs = set->wrkrs->right;
			if (clist_rm_tail(&set->wrkrs, false) == true)
				set->cnt -= 1;
		}
		else
		{
			set->wrkrs = set->wrkrs->right;
			wrkr->stat.running = true;
			printf("connected to %s\n", inet_ntoa(wrkr->sin.sin_addr));
		}
		iter--;
	}
	return (set->cnt);
}

void				send_workers_binn(t_workerset *set, binn *obj, uint8_t code)
{
	t_wrkr			*wrkr;
	uint32_t 		iter;

	iter = set->cnt;
	while (iter)
	{
		wrkr = set->wrkrs->data;
		hermes_send_binn(wrkr->sock, code, obj);
		set->wrkrs = set->wrkrs->right;
		iter--;
	}
}

void				send_workers_initial_env(t_mgr *mgr)
{
	binn			*opts;
	binn			*ports;
	binn			*ack_ports;
	binn			*syn_ports;
	binn			*udp_ports;

	opts = binnify_opts(mgr->job.opts);
	send_workers_binn(mgr->workers, opts,
					  C_OBJ_OPTS);
	free(opts);
	ports = binnify_portset(mgr->job.ports);
	send_workers_binn(mgr->workers, ports,
					  C_OBJ_PS_NRM);
	free(ports);
	if (mgr->job.ack_ports) {
		ack_ports = binnify_portset(mgr->job.ack_ports);
		send_workers_binn(mgr->workers, ack_ports,
						  C_OBJ_PS_ACK);
		free(ack_ports);
	}
	if (mgr->job.syn_ports) {
		syn_ports = binnify_portset(mgr->job.syn_ports);
		send_workers_binn(mgr->workers, syn_ports,
						  C_OBJ_PS_SYN);
		free(syn_ports);
	}
	if (mgr->job.udp_ports) {
		udp_ports = binnify_portset(mgr->job.udp_ports);
		send_workers_binn(mgr->workers, udp_ports,
						  C_OBJ_PS_ACK);
		free(udp_ports);
	}
}

int					manager_loop(t_mgr *mgr)
{
	struct protoent	*proto;

	mgr->stat.running = true;
	if ((proto = getprotobyname("tcp")) == 0)
		return (FAILURE);
	mgr->workers->wrkrs = tree_to_list(&mgr->workers->wrkrs);
	if (mgr->workers && mgr->workers->cnt > 0)
	{
		if ((mgr->workers->cnt = connect_workers(
				mgr->workers, proto->p_proto)) > 0)
		{
			printf("connected to %i worker(s).\n", mgr->workers->cnt);
			send_workers_initial_env(mgr);
		}
		else
			printf("failed to connected to any workers...\n");
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
			{

			}
			else
				mgr->stat.running = false;
		}
//		else if (mgr->job.targets->total > 0 && !mgr->cwork)
//		{
//			mgr->cwork =
//		}
		while (mgr->workers->wrkrs)
		{

		}
	}
	return (0);
}
