#include "../incl/hermes.h"
#include "../incl/binnify.h"
#include "../incl/message.h"

struct pollfd	*new_fds(uint32_t count)
{
	struct pollfd *fds;

	if (!(fds = (struct pollfd*)memalloc(
			sizeof(struct pollfd) * count)))
		hermes_error(FAILURE, "malloc()");
	return (fds);
}

void		connect_workers(t_node **workers, t_workerset *set, int proto)
{
	t_wrkr		*worker;

	if (!*workers)
		return ;
	if ((*workers)->left)
		connect_workers(&(*workers)->left, set, proto);
	worker = (t_wrkr*)(*workers)->data;
	if ((worker->sock = socket(PF_INET, SOCK_STREAM, proto)) == -1)
		hermes_error(EXIT_FAILURE, "socket() %s", strerror(errno));
	if (connect(worker->sock, (const struct sockaddr *)&worker->sin, sizeof(worker->sin)) == -1)
	{
		hermes_error(FAILURE, "could not connect to worker. dropping: %s", inet_ntoa(worker->sin.sin_addr));
		if (rm_node_bst(&set->wrkrs, worker, worker_cmp, worker_min) == true)
			set->cnt -= 1;
	}
	else
	{
		worker->stat.running = true;
		printf("connected to %s.\n", inet_ntoa(worker->sin.sin_addr));
	}
	if ((*workers)->right)
		connect_workers(&(*workers)->right, set, proto);
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
	if (mgr->job.ack_ports)
	{
		ack_ports = binnify_portset(mgr->job.ack_ports);
		send_workers_binn(mgr->workers, ack_ports,
						  C_OBJ_PS_ACK);
		free(ack_ports);
	}
	if (mgr->job.syn_ports)
	{
		syn_ports = binnify_portset(mgr->job.syn_ports);
		send_workers_binn(mgr->workers, syn_ports,
						  C_OBJ_PS_SYN);
		free(syn_ports);
	}
	if (mgr->job.udp_ports)
	{
		udp_ports = binnify_portset(mgr->job.udp_ports);
		send_workers_binn(mgr->workers, udp_ports,
						  C_OBJ_PS_ACK);
		free(udp_ports);
	}
}

void				transfer_all_work(t_targetset *dst, t_targetset *src) {
	t_ip4 *ip4;
	t_ip4rng *rng;

	if (!src || !dst)
		return;
	src->total += dst->total;
	src->ip_cnt += dst->ip_cnt;
	src->rng_cnt += dst->rng_cnt;
	while (src->ips) {
		ip4 = new_ip4();
		memcpy(ip4, src->ips->data, sizeof(t_ip4));
		add_node_bst(&dst->ips, (void **) &ip4, ip4_cmp);
		rm_node_bst(&src->ips, ip4, ip4_cmp, ip4_min);
	}
	while (src->iprngs)
	{
		rng = new_ip4range();
		memcpy(rng, src->iprngs, sizeof(t_ip4rng));
		add_node_bst(&dst->iprngs, (void **)&rng, ip4rng_cmp);
		rm_node_bst(&src->iprngs, rng, ip4rng_cmp, ip4rng_min);
	}
}

//bool				handle_disconnect(t_mgr **mgr, t_wrkr **wrkr)
//{
//	/* TODO :
//	 * try 				: ask worker to dump all results
//	 * try 				: ask worker to dump all unfinished work
//	 * if results 		: append results to results queue
//	 * if unfin work 	: append unfinished work to another worker
//	 * disconnect
//	 * remove worker from list
//	 */
//	return (true);
//}

bool				send_work(t_mgr *mgr, t_wrkr *wrkr)
{
	(void)wrkr;
	(void)mgr;
	return (true);
}

int 				mgr_process_msg(t_mgr *mgr, t_wrkr *wrkr, uint8_t *msgbuff)
{
	t_msg_hdr	*hdr;

	if (!wrkr)
		return (FAILURE);
	hdr = (t_msg_hdr*)msgbuff;
	printf("type: %i code: %i\n", hdr->type, hdr->code);
	bzero(msgbuff, PKT_SIZE);
	if (hdr->type == T_WRK_REQ) // TODO : if (NEEDS WORK) {send work}
	{
		if (mgr->job.targets->total)
		{
			if (send_work(mgr, wrkr) == false)
				return (FAILURE);
		}
	}
//	else if (hdr->type == T_SHUTDOWN)
//	{
//		if (handle_disconnect(&mgr, &wrkr) == false)
//			transfer_all_work(mgr->job.targets, wrkr->job->targets);
//	}
	return (SUCCESS);
}

void			loop_get_max_fd(t_node *wrkrs, int *fdmax)
{
	t_wrkr		*wrkr;

	if (!wrkrs)
		return;
	if (wrkrs->left)
		loop_get_max_fd(wrkrs->left, fdmax);
	wrkr = wrkrs->data;
	if (wrkr->sock > *fdmax)
		*fdmax = wrkr->sock;
	if (wrkrs->right)
		loop_get_max_fd(wrkrs->right, fdmax);
}

int				get_max_fd(t_workerset *set)
{
	int			fdmax;

	fdmax = -1;
	loop_get_max_fd(set->wrkrs, &fdmax);
	return (fdmax);
}

void				check_workers(t_mgr *mgr, struct pollfd *fds)
{
	ssize_t 	rc;
	uint32_t	iter;
	uint8_t		msgbuff[PKT_SIZE];

	iter = mgr->workers->cnt;
	rc = poll(fds, mgr->workers->cnt, 500);
	if (rc < 0)
		return; // TODO
	else if (rc == 0)
		printf("timeout\n");
	else
	{
		while (iter)
		{
			/*
			 * if NEEDS WORK:
			 * 		batch work
			 * 		send msg
			 * 		send work
			 * 	if WRKR SHUT DOWN:
			 * 		recv work
			 * 		redistribute to next available worker
			 */
			if (fds[iter].revents)
				if (hermes_recvmsg(fds[iter].fd, msgbuff) > 0)
					mgr_process_msg(
							mgr,
							((t_wrkr *) mgr->workers->wrkrs[fds[iter].fd].data),
							msgbuff);
			/* TODO : else () worker did not send anything */
			iter--;
		}
	}
}

void				add_wrkrs_to_array(t_node *wrkr, t_wrkr *array)
{
	t_wrkr		*w;

	if (!wrkr)
		return ;
	if (wrkr->left)
		add_wrkrs_to_array(wrkr->left, array);
	w = (t_wrkr*)wrkr->data;
	memcpy(&array[w->sock], w, sizeof(t_wrkr));
	if (wrkr->right)
		add_wrkrs_to_array(wrkr->right, array);
}

t_node		*wrkrtree_to_fdinxarray(t_node **wrkrtree, int maxfd)
{
	t_wrkr *array;

	array = memalloc(sizeof(t_wrkr) * (maxfd + 1));
	if (!array)
	{
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
		return (NULL);
	}
	add_wrkrs_to_array(*wrkrtree, array);
	del_tree(wrkrtree, true);
	return (new_node((void **)&array));
}

/*TODO we need to heapify targets before they reach this function.
 * 1. make all heapify functions
 * 2. add heapify calls on ip/port trees in sanity check.
 * 3. if no ports are specified have a port heap made.
 *
 * */
int					manager_loop(t_mgr *mgr)
{
	int				maxfd;
	struct pollfd	*fds;
	struct protoent	*proto;

	fds = NULL;
	mgr->stat.running = true;
	if ((proto = getprotobyname("tcp")) == 0)
		return (FAILURE);
	if (mgr->workers && mgr->workers->cnt > 0)
	{
		connect_workers(&mgr->workers->wrkrs, mgr->workers, proto->p_proto);
		if (mgr->workers->cnt > 0)
		{
			printf("connected to %i worker%s\n", mgr->workers->cnt, (mgr->workers->cnt == 1)? "s.":".");
			send_workers_initial_env(mgr);
			if (!(maxfd = get_max_fd(mgr->workers)))
				hermes_error(FAILURE, "get_max_fd()");
			mgr->workers->wrkrs = wrkrtree_to_fdinxarray(&mgr->workers->wrkrs, maxfd);
		}
		else
			printf("failed to connected to any workers...\n");
	}
	/* TODO : put this into a new function(mgr, fds) */
	/* TODO Spawn thread pool */
	while (mgr->stat.running == true)
	{
		/* TODO grab "normal" chunk of ips from mgr->job and assign them to mgr->workers, and mgr->cwork */
		/* TODO split mgr->cwork into smaller target_sets and assign to targetset_list for threads */
		/* TODO pass divided work to threads */
		if (mgr->workers && mgr->workers->wrking_cnt > 0)
			check_workers(mgr, fds);
		else if (mgr->job.targets->total > 0 && !mgr->cwork)
		{
			partition_targetset(mgr->cwork, mgr->job.targets, 100);
		}
		if (mgr->job.targets->total == 0 && mgr->workers && mgr->workers->wrking_cnt == 0)
			mgr->stat.running = false;
	}
	return (0);
}
