#include "../incl/hermes.h"
#include "../incl/binnify.h"
#include "../incl/message.h"

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
		if (worker->sock > (int)set->maxfd)
			set->maxfd = (nfds_t)worker->sock;
		worker->stat.running = true;
		printf("connected to %s\n", inet_ntoa(worker->sin.sin_addr));
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
	binn 			*job;

	job = binnify_env(&mgr->env);
	send_workers_binn(&mgr->workers, job, C_OBJ_ENV);
	free(job);
}

int					send_work(t_wrkr *worker)
{
	binn			*targets;

	targets = binnify_targetset(&worker->targets);
	hermes_send_binn(worker->sock, C_OBJ_TARGETS, targets);
	return (SUCCESS);
}

int					handle_result_offer(t_mgr *mgr, t_wrkr *worker,
										   uint8_t *msg)
{
	ssize_t			ret;
	binn			*obj;
	t_obj_hdr		*hdr;

	printf("Entering handle_result_offer()\n");
	if (!mgr || !worker || !msg)
		return (FAILURE);
	hdr = (t_obj_hdr *)msg;
	hdr->objlen = ntohl(hdr->objlen);
	printf("%u\n", hdr->objlen);
	if (hdr->objlen <= 0)
		return (FAILURE);
	if (!(obj = (binn *)malloc(sizeof(uint8_t) * hdr->objlen)))
		return (hermes_error(FAILURE, "malloc() %s", strerror(errno)));
	ret = recv(worker->sock, obj, hdr->objlen, MSG_WAITALL);
	if (ret == 0)
	{
		free(obj);
		worker->stat.running = false;
		return (FAILURE);
	}
	if (hdr->code == C_OBJ_RES)
	{
		if (mgr->tpool)
			pthread_mutex_lock(&mgr->tpool->results_mtx);
		printf("got mutex\n");
		unbinnify_resultset(&mgr->results, &worker->targets, obj);
		if (mgr->tpool)
			pthread_mutex_unlock(&mgr->tpool->results_mtx);
		printf("gave mutex\n");
	}
	free(obj);
	printf("Leaving handle_result_offer()\n");
	return (SUCCESS);
}

int					mgr_process_msg(t_mgr *mgr, t_wrkr *wrkr, uint8_t *msgbuff)
{
	t_msg_hdr		*hdr;

	if (!wrkr)
		return (FAILURE);
	hdr = (t_msg_hdr*)msgbuff;
	printf("type: %i code: %i\n", hdr->type, hdr->code);
	if (hdr->type == T_WRK_REQ && hdr->code == C_WRK_REQ)
	{
		if (mgr->targets.total > 0)
		{
			wrkr->targets.total = 0;
			wrkr->targets.ip_cnt = 0;
			wrkr->targets.rng_cnt = 0;
			transfer_work(&wrkr->targets, &mgr->targets, wrkr->send_size);
			wrkr->send_size *= 2;
			send_work(wrkr);
			wrkr->stat.has_work = true;
			mgr->workers.wrking_cnt += 1;
		}
		else
		{
			hermes_sendmsgf(wrkr->sock, msg_tc(T_SHUTDOWN, C_SHUTDOWN_SFT), NULL);
			wrkr->stat.running = false;
			wrkr->stat.has_work = false;
			wrkr->set->wrking_cnt -= 1;
			/*TODO this doesn't work properly... gotta */
			((t_wrkr **)wrkr->set->wrkrs->data)[wrkr->sock] = NULL;
			free(wrkr);
		}
	}
	else if (hdr->type == T_OBJ && hdr->code == C_OBJ_RES)
	{
		if (handle_result_offer(mgr, wrkr, msgbuff) == FAILURE)
			return (FAILURE);
	}
	return (SUCCESS);
}

int poll_wrkr_msgs(t_mgr *mgr, nfds_t fditer,
                   struct pollfd *fds)
{
	uint8_t			msgbuff[PKT_SIZE];
	t_wrkr			**workers;
	ssize_t 		ret;

	printf("check workers called\n");
	workers = mgr->workers.wrkrs->data;
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
					if (mgr_process_msg(mgr, workers[fds[fditer].fd], msgbuff) == FAILURE)
						return (FAILURE);
			}
		}
	}
	return (SUCCESS);
}

void				add_wrkrtree_to_array(t_node *wrkr, t_wrkr **array)
{
	t_wrkr			*w;

	if (!wrkr)
		return ;
	if (wrkr->left)
		add_wrkrtree_to_array(wrkr->left, array);
	w = (t_wrkr*)wrkr->data;
	array[w->sock] =  w;
	if (wrkr->right)
		add_wrkrtree_to_array(wrkr->right, array);
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
	add_wrkrtree_to_array(*wrkrtree, array);
	del_tree(wrkrtree, false);
	return (new_node((void **)&array));
}

void				run_scan(t_env *env, t_targetset *targets,
                             t_resultset *res_ptr, pthread_mutex_t *res_mtx)
{
	t_result	*result;
	t_ip4rng	*curr;

	if (!env || !targets || !res_ptr)
		return ;
	if (targets->total > 0)
	{
		while (targets->ips)
		{
//			sleep(1);
			if (!(result = new_result()))
				return ;
			result->ip = *(t_ip4*)targets->ips->data;
			pthread_mutex_lock(res_mtx);
			clist_add_head(&res_ptr->results, (void**)&result);
			res_ptr->result_cnt += 1;
			res_ptr->byte_size += sizeof(result);
			pthread_mutex_unlock(res_mtx);
			if (clist_rm_head(&targets->ips, true) == false)
				break;
			targets->total--;
			targets->ip_cnt--;
			printf("worked on ip %s\n", inet_ntoa(result->ip));
		}
		while (targets->iprngs)
		{
			curr = (t_ip4rng *)targets->iprngs->data;
			while (ntohl(curr->start) <= ntohl(curr->end))
			{
//				sleep(1);
				if (!(result = (t_result*)memalloc(sizeof(t_result))))
					return;
				result->ip.s_addr = curr->start;
				pthread_mutex_lock(res_mtx);
				clist_add_head(&res_ptr->results, (void **) &result);
				res_ptr->result_cnt += 1;
				res_ptr->byte_size += sizeof(result);
				pthread_mutex_unlock(res_mtx);
				curr->start = ip4_increment(curr->start, 1);
				targets->total--;
				printf("worked on ip %u\n", result->ip.s_addr);
			}
			if (clist_rm_head(&targets->iprngs, true) == false)
				break;
			targets->rng_cnt--;
		}
	}
}

int					init_workers(t_mgr *mgr, struct pollfd **fds)
{
	size_t			i;
	struct protoent	*proto;
	t_wrkr			**workers;

	if ((proto = getprotobyname("tcp")) == 0)
		hermes_error(EXIT_FAILURE, "getprotobyname() %s", strerror(errno));
	connect_workers(&mgr->workers.wrkrs, &mgr->workers, proto->p_proto);
	if (mgr->workers.cnt > 0)
	{
		mgr->workers.maxfd += 1;
		printf("connected to %i worker%s\n", mgr->workers.cnt, (mgr->workers.cnt == 1) ? ".":"s.");
		mgr->workers.wrkrs = wrkrtree_to_fdinxarray(&mgr->workers.wrkrs, mgr->workers.maxfd);
		workers = mgr->workers.wrkrs->data;
		*fds = memalloc(sizeof(struct pollfd) * mgr->workers.maxfd);
		for (i = 0; i < mgr->workers.maxfd; i++)
		{
			if (workers[i])
			{
				(*fds)[workers[i]->sock].fd = workers[i]->sock;
				(*fds)[workers[i]->sock].events = POLLIN;
			}
		}
		send_workers_initial_env(mgr);
	}
	else
	{
		del_tree(&mgr->workers.wrkrs, true);
		printf("failed to connected to any workers...\n");
	}
	return (SUCCESS);
}

void				tend_threads(t_mgr *mgr)
{
	pthread_mutex_lock(&mgr->tpool->amt_working_mtx);
	if (mgr->tpool->amt_working != mgr->tpool->tcount)
	{
		pthread_mutex_unlock(&mgr->tpool->amt_working_mtx);
		pthread_mutex_lock(&mgr->tpool->work_pool_mtx);
		if (mgr->tpool->work_pool->total == 0)
		{
			transfer_work(&mgr->thread_targets, &mgr->targets, mgr->tpool->reqest_amt);
			mgr->tpool->reqest_amt *= (mgr->tpool->reqest_amt < 2048) ? 2 : 1;
			tpool_event(mgr->tpool);
		}
		pthread_mutex_unlock(&mgr->tpool->work_pool_mtx);
	}
	else
		pthread_mutex_unlock(&mgr->tpool->amt_working_mtx);
	pthread_mutex_lock(&mgr->tpool->results_mtx);
	while (mgr->tpool->results->results != NULL)
	{
		clist_add_head(&mgr->results.results,
		               &mgr->tpool->results->results->data);
		clist_rm_head(&mgr->tpool->results->results, true);
		mgr->tpool->results->result_cnt -= 1;
		mgr->tpool->results->byte_size -= sizeof(mgr->tpool->results->byte_size);
		mgr->results.result_cnt += 1;
		mgr->results.byte_size += sizeof(mgr->results.results->data);
	}
	pthread_mutex_unlock(&mgr->tpool->results_mtx);
}

bool				check_if_finished(t_mgr *mgr)
{
	if (mgr->targets.total > 0)
		return (false);
	if (mgr->workers.wrking_cnt != 0)
		return (false);
	if (mgr->tpool)
	{
		pthread_mutex_lock(&mgr->tpool->amt_working_mtx);
		if (mgr->tpool->amt_working != 0)
		{
			pthread_mutex_unlock(&mgr->tpool->amt_working_mtx);
			return (false);
		}
		pthread_mutex_unlock(&mgr->tpool->amt_working_mtx);
	}
	return (true);
}

void				check_results(t_mgr *mgr)
{
	t_result		*res;

//	printf("result count: %d\n", mgr->results.result_cnt);
	if (mgr->results.result_cnt > 0)
	{
		res = mgr->results.results->data;
		printf("result: %s\n", inet_ntoa(res->ip));
		// TODO
		clist_rm_head(&mgr->results.results, false);
		mgr->results.result_cnt--;
	}
}

int					manager_loop(t_mgr *mgr)
{
	struct pollfd	*fds;
	t_resultset		results;

	fds = NULL;
	memset(&results, 0, sizeof(t_resultset));
	mgr->stat.running = true;
	if (mgr->env.opts.thread_count > 0)
	{
		if (!(mgr->tpool = init_threadpool(&mgr->env, &mgr->thread_targets, &results)))
			hermes_error(EXIT_FAILURE, "init_threadpool()");
	}
	if (mgr->workers.cnt > 0)
	{
		init_workers(mgr, &fds);
	}
	while (mgr->stat.running == true)
	{
		/* if we have workers, see if they've sent us any messages */
		if (mgr->workers.cnt > 0)
			if (poll_wrkr_msgs(mgr, mgr->workers.maxfd, fds) == FAILURE)
				hermes_error(FAILURE, "poll_wrkr_msgs");
		if (mgr->tpool)
			tend_threads(mgr);
		check_results(mgr);
		if (check_if_finished(mgr) == true)
			mgr->stat.running = false;
	}
	if (mgr->tpool)
		tpool_kill(mgr->tpool);
	return (SUCCESS);
}
