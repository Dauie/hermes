#include "../incl/hermes.h"
#include "../incl/binnify.h"
#include "../incl/message.h"

struct pollfd		*new_fds(uint32_t count)
{
	struct pollfd	*fds;

	if (!(fds = (struct pollfd*)memalloc(
			sizeof(struct pollfd) * count)))
		hermes_error(FAILURE, "malloc()");
	return (fds);
}

void				connect_workers(t_node **workers, t_workerset *set, int proto)
{
	t_wrkr			*worker;

	if (!*workers)
		return ;
	if (*workers && (*workers)->left)
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
	if (*workers && (*workers)->right)
		connect_workers(&(*workers)->right, set, proto);
}

void				send_workers_binn(t_workerset *set, binn *obj, uint8_t code)
{
	t_wrkr			**wrkrs;
	size_t			i;

	i = 0;
	wrkrs = (t_wrkr **)set->wrkrs->data;
	while (i++ < set->maxfd)
	{
		if (wrkrs[i])
			hermes_send_binn(wrkrs[i]->sock, code, obj);
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
	send_workers_binn(mgr->workers, opts, C_OBJ_OPTS);
	free(opts);
	ports = binnify_portset(mgr->job.ports);
	send_workers_binn(mgr->workers, ports, C_OBJ_PS_NRM);
	free(ports);
	if (mgr->job.ack_ports)
	{
		ack_ports = binnify_portset(mgr->job.ack_ports);
		send_workers_binn(mgr->workers, ack_ports, C_OBJ_PS_ACK);
		free(ack_ports);
	}
	if (mgr->job.syn_ports)
	{
		syn_ports = binnify_portset(mgr->job.syn_ports);
		send_workers_binn(mgr->workers, syn_ports, C_OBJ_PS_SYN);
		free(syn_ports);
	}
	if (mgr->job.udp_ports)
	{
		udp_ports = binnify_portset(mgr->job.udp_ports);
		send_workers_binn(mgr->workers, udp_ports, C_OBJ_PS_ACK);
		free(udp_ports);
	}
}

int					send_work(t_wrkr *worker)
{
	binn			*targets;

	targets = binnify_targetset(worker->job->targets);
	hermes_send_binn(worker->sock, C_OBJ_TARGETS, targets);
	return (SUCCESS);
}


int 				mgr_process_msg(t_mgr *mgr, t_wrkr *wrkr, uint8_t *msgbuff)
{
	t_msg_hdr		*hdr;

	if (!wrkr)
		return (FAILURE);
	hdr = (t_msg_hdr*)msgbuff;
	printf("type: %i code: %i\n", hdr->type, hdr->code);
	if (hdr->type == T_WRK_REQ)
	{
		if (mgr->job.targets->total > 0)
		{
			if (!wrkr->job)
			{
				wrkr->job = new_job();
				wrkr->job->targets = new_targetset();
			}
			if (wrkr->job->targets->total == 0)
			{
				transfer_work(wrkr->job->targets, mgr->job.targets, 20);
				send_work(wrkr);
				mgr->workers->wrking_cnt += 1;
			}
		}
		else
			hermes_sendmsgf(wrkr->sock, msg_tc(T_SHUTDOWN, C_SHUTDOWN_SFT), NULL);
	}
	else if (hdr->type == T_OBJ && hdr->code == C_OBJ_RES)
	{

	}
//	else if (hdr->type == T_SHUTDOWN)
//	{
//		if (handle_disconnect(&mgr, &wrkr) == false)
//			transfer_all_work(mgr->job.targets, wrkr->job->targets);
//	}
	return (SUCCESS);
}

void				loop_get_max_fd(t_node *wrkrs, nfds_t *fdmax)
{
	t_wrkr			*wrkr;

	if (!wrkrs)
		return;
	if (wrkrs->left)
		loop_get_max_fd(wrkrs->left, fdmax);
	wrkr = wrkrs->data;
	if (wrkr->sock > (int)*fdmax)
		*fdmax = (nfds_t)wrkr->sock;
	if (wrkrs->right)
		loop_get_max_fd(wrkrs->right, fdmax);
}

nfds_t				get_max_fd(t_workerset *set)
{
	nfds_t			fdmax;

	fdmax = 0;
	loop_get_max_fd(set->wrkrs, &fdmax);
	return (fdmax + 1);
}

void				check_workers(t_mgr *mgr, nfds_t fditer, struct pollfd *fds)
{
	uint8_t			msgbuff[PKT_SIZE];
	t_wrkr			**workers;
	ssize_t 		ret;

	printf("check workers called\n");
	workers = mgr->workers->wrkrs->data;
	ret = poll(fds, fditer, 500);
	if (ret < 0)
	{
		if (errno != EAGAIN && errno != EINTR)
		{
			mgr->stat.running = false;
			hermes_error(FAILURE, "poll()", strerror(errno));
		}
	}
	else if (ret == 0)
		printf("timeout\n");
	else
	{
		printf("poll says mgr got a message\n");
		while (fditer--)
		{
			if (fds[fditer].revents & POLLIN)
			{
				printf("entering to recv msg\n");
				if (hermes_recvmsg(fds[fditer].fd, msgbuff) > 0)
					mgr_process_msg(mgr, workers[fds[fditer].fd], msgbuff);
			}
		}
	}
}

void				add_wrkrstree_to_array(t_node *wrkr, t_wrkr **array)
{
	t_wrkr			*w;

	if (!wrkr)
		return ;
	if (wrkr->left)
		add_wrkrstree_to_array(wrkr->left, array);
	w = (t_wrkr*)wrkr->data;
	array[w->sock] =  w;
	if (wrkr->right)
		add_wrkrstree_to_array(wrkr->right, array);
}


t_node				*wrkrtree_to_fdinxarray(t_node **wrkrtree, nfds_t maxfd)
{
	t_wrkr			**array;

	array = memalloc(sizeof(t_wrkr *) * (maxfd + 1));
	if (!array)
	{
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
		return (NULL);
	}
	add_wrkrstree_to_array(*wrkrtree, array);
	del_tree(wrkrtree, false);
	return (new_node((void **)&array));
}

void				threads_go(t_thread thread, t_result **results)
{
	uint16_t tcnt;

	tcnt = tpool.thr_count;
	while (--tcnt > 0)
	{
		if (tpool.threads[tcnt]->alive &&
			!tpool.threads[tcnt]->working)
		{
			/* TODO :
			 * lock results
			 * run scan
			 *
			 *
			 */
		}
	}
}

int					manager_loop(t_mgr *mgr)
{
	struct pollfd	*fds;
	struct protoent	*proto;
	t_wrkr			**workers;
	t_thrpool		tpool;
	t_result		*results;


	fds = NULL;
	mgr->workers->maxfd = 0;
	mgr->stat.running = true;
	if ((proto = getprotobyname("tcp")) == 0)
		return (FAILURE);
	if (mgr->job.opts->thread_count > 0)
	{
		tpool.thr_count = mgr->job.opts->thread_count;
		tpool.threads = memalloc(sizeof(t_thread) * tpool.thr_count);
		for (int i = 0; i < mgr->job.opts->thread_count; i++)
		{
			tpool.threads[i].thread = pthread_create()
		}
	}
	if (mgr->workers && mgr->workers->cnt > 0)
	{
		connect_workers(&mgr->workers->wrkrs, mgr->workers, proto->p_proto);
		if (mgr->workers->cnt > 0)
		{
			printf("connected to %i worker%s\n", mgr->workers->cnt, (mgr->workers->cnt == 1) ? ".":"s.");
			if (!(mgr->workers->maxfd = get_max_fd(mgr->workers)))
				hermes_error(FAILURE, "get_max_fd()");
			mgr->workers->wrkrs = wrkrtree_to_fdinxarray(&mgr->workers->wrkrs, mgr->workers->maxfd);
			workers = mgr->workers->wrkrs->data;
			fds = memalloc(sizeof(struct pollfd) * mgr->workers->maxfd);
			for (size_t i = 0; i < mgr->workers->maxfd; i++)
			{
				if (workers[i])
				{
					fds[workers[i]->sock].fd = workers[i]->sock;
					fds[workers[i]->sock].events = POLLIN;
				}
			}
			send_workers_initial_env(mgr);
		}
		else
			printf("failed to connected to any workers...\n");
	}


	while (mgr->stat.running == true)
	{
		/* if we have workers, see if they've sent us any messages */
		if (mgr->workers && mgr->workers->cnt > 0)
		{
			check_workers(mgr, mgr->workers->maxfd, fds);
		}
		/*
		** If our main job is not empty && our local threads do not
		** have work, transfer work to cwork for threads to process
		*/
		if (mgr->job.targets->total > 0 && (!mgr->thread_work || mgr->thread_work->total == 0))
		{
			if (!mgr->thread_work)
				mgr->thread_work = new_targetset();
			transfer_work(mgr->thread_work, mgr->job.targets, 20);
		}
		if (mgr->job.targets->total == 0 && mgr->thread_work->total == 0)
		{
			if (mgr->workers && mgr->workers->wrking_cnt != 0)
				continue;
			else
				mgr->stat.running = false;
		}
	}
	return (0);
}
