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
	binn 			*job;

	job = binnify_job(&mgr->job);
	send_workers_binn(&mgr->workers, job, C_OBJ_JOB);
	free(job);
}

int					send_work(t_wrkr *worker)
{
	binn			*targets;

	targets = binnify_targetset(&worker->targets);
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
		if (mgr->targets.total > 0)
		{
			if (wrkr->targets.total == 0)
			{
				transfer_work(&wrkr->targets, &mgr->targets, 20);
				send_work(wrkr);
				wrkr->stat.has_work = true;
				mgr->workers.wrking_cnt += 1;
			}
		}
		else
		{
			hermes_sendmsgf(wrkr->sock, msg_tc(T_SHUTDOWN, C_SHUTDOWN_SFT), NULL);
			wrkr->stat.has_work = false;
			mgr->workers.wrking_cnt -= 1;

		}
	}
	else if (hdr->type == T_OBJ && hdr->code == C_OBJ_RES)
	{

	}
//	else if (hdr->type == T_SHUTDOWN)
//	{
//		if (handle_disconnect(&mgr, &wrkr) == false)
//			transfer_all_work(mgr->targets.targets, wrkr->targets->targets);
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

void				poll_wrkr_msgs(t_mgr *mgr, nfds_t fditer,
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
					mgr_process_msg(mgr, workers[fds[fditer].fd], msgbuff);
			}
		}
	}
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

//void 				run_thr_scan(t_job *job, t_targetset *targetset, t_result **res_ptr,
//									pthread_mutex_t *res_mtx)
//{
//
//}

void				run_scan(t_job *job, t_targetset *targets,
							 t_node **res_ptr,
							 pthread_mutex_t *res_mtx)
{
	t_ip4		*ip;
	t_result	*result;
	t_ip4rng	*curr;

	if (!job || !targets || !res_ptr)
		return ;
	if (res_mtx)
	{
		if (targets->total > 0)
		{
			while (targets->ips)
			{
				sleep(10);
				if (!(result = (t_result*)memalloc(sizeof(t_result))))
					return ;
				result->ip = *(t_ip4 *) targets->ips->data;
				pthread_mutex_lock(res_mtx);
				clist_add_head(res_ptr, (void**)&result);
				pthread_mutex_unlock(res_mtx);
				if (clist_rm_head(&targets->ips, true) == false)
					break;
				targets->total--;
				targets->ip_cnt--;
			}
			while (targets->iprngs)
			{
				curr = (t_ip4rng *) targets->iprngs->data;
				while (ntohl(curr->start) <= ntohl(curr->end))
				{
					ip = new_ip4();
					ip->s_addr = curr->start;

					sleep(10);
					if (!(result = (t_result *) memalloc(sizeof(t_result))))
						return;
					result->ip = *ip;
					pthread_mutex_lock(res_mtx);
					clist_add_head(res_ptr, (void **) &result);
					pthread_mutex_unlock(res_mtx);
					curr->start = ip4_increment(curr->start, 1);
					targets->total--;
					if (ip)
						free(ip);
				}
				if (clist_rm_head(&targets->iprngs, true) == false)
					break;
				targets->rng_cnt--;
			}
		}
	}
}

void				*thread_loop(void *thrd)
{
	t_targetset		work;
	t_thread		*thread;

	thread = (t_thread *)thrd;
	memset(&work, 0, sizeof(t_targetset));
	while (thread->alive)
	{
			pthread_mutex_lock(&thread->pool->work_pool_mutex);
			if (thread->pool->work_pool->total > 0)
			{
				transfer_work(&work, thread->pool->work_pool, thread->amt);
				pthread_mutex_unlock(&thread->pool->work_pool_mutex);
				thread->amt *= (thread->amt < 512) ? 2 : 1;
				thread->working = true;
				pthread_mutex_lock(&thread->pool->amt_working_mutex);
				thread->pool->amt_working += 1;
				pthread_mutex_unlock(&thread->pool->amt_working_mutex);
				run_scan(thread->pool->job, &work,
						&thread->pool->results, &thread->pool->results_mutex);
				thread->working = false;
				pthread_mutex_lock(&thread->pool->amt_working_mutex);
				thread->pool->amt_working -= 1;
				pthread_mutex_unlock(&thread->pool->amt_working_mutex);
			}
			else
			{
				pthread_mutex_unlock(&thread->pool->work_pool_mutex);
				sleep(1);
			}
	}
	return (NULL);
}

int					init_workers(t_mgr *mgr, struct pollfd **fds)
{
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
		for (size_t i = 0; i < mgr->workers.maxfd; i++)
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

void				kill_threadpool(t_thrpool *pool)
{
	int				i;

	i = -1;
	if (!pool)
		return ;
	while (++i < pool->thr_count)
		pool->threads[i].alive = false;
	sleep(1);
	if (pool->threads)
		free(pool->threads);
//	del_clist(&pool->work_pool->ips, true);
//	del_clist(&pool->work_pool->iprngs, true);
}

t_thrpool			*init_threadpool(t_job *job, t_targetset *workpool,
									  t_node **results)
{
	int					i;
	t_thrpool			*pool;

	i = -1;
	if (!(pool = memalloc(sizeof(t_thrpool))))
		return (NULL);
	if (!(pool->threads = memalloc(sizeof(t_thread) * job->opts.thread_count)))
		return (NULL);
	pthread_mutex_init(&pool->work_pool_mutex, NULL);
	pthread_mutex_init(&pool->results_mutex, NULL);
	pthread_mutex_init(&pool->amt_working_mutex, NULL);
	pool->thr_count = job->opts.thread_count;
	pool->reqest_amt = pool->thr_count;
	pool->results = *results;
	pool->work_pool = workpool;
	pool->job = job;
	while (++i < pool->thr_count)
	{
		pool->threads[i].amt = 1;
		pool->threads[i].pool = pool;
		pool->threads[i].alive = true;
		pool->threads[i].working = false;
		pthread_create(&pool->threads[i].thread, NULL, thread_loop,
					   &pool->threads[i]);
	}
	return (pool);
}

int					manager_loop(t_mgr *mgr)
{
	struct pollfd	*fds;
	t_thrpool		*tpool;
	t_node			*results;

	tpool = NULL;
	fds = NULL;
	results = NULL;
	mgr->stat.running = true;
	if (mgr->job.opts.thread_count > 0)
	{
		if (!(tpool = init_threadpool(&mgr->job, &mgr->thrd_targets, &results)))
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
		{
			poll_wrkr_msgs(mgr, mgr->workers.maxfd, fds);
		}
		if (tpool)
		{
			pthread_mutex_lock(&tpool->amt_working_mutex);
			if (tpool->amt_working != tpool->thr_count)
			{
				pthread_mutex_unlock(&tpool->amt_working_mutex);
				pthread_mutex_lock(&tpool->work_pool_mutex);
				if (tpool->work_pool->total == 0)
				{
					transfer_work(&mgr->thrd_targets, &mgr->targets, tpool->reqest_amt);
					tpool->reqest_amt *= (tpool->reqest_amt < 2048) ? 2 : 1;
				}
				pthread_mutex_unlock(&tpool->work_pool_mutex);
			}
			else
				pthread_mutex_unlock(&tpool->amt_working_mutex);
		}
		if (mgr->targets.total == 0) /*TODO account for threads*/
		{
			if (mgr->workers.wrking_cnt != 0)
				continue;
			pthread_mutex_lock(&tpool->amt_working_mutex);
			if (tpool->amt_working != 0)
			{
				pthread_mutex_unlock(&tpool->amt_working_mutex);
				continue;
			}
			pthread_mutex_unlock(&tpool->amt_working_mutex);
			mgr->stat.running = false;
		}
	}
	if (tpool)
		kill_threadpool(tpool);
	return (0);
}
