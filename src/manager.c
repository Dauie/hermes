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
	binn 			*env;

	env = binnify_env(&mgr->env);
	send_workers_binn(&mgr->workers, env, C_OBJ_ENV);
	binn_free(env);
}

int					send_work(t_wrkr *worker)
{
	binn			*targets;

	targets = binnify_targetset(&worker->targets);
	hermes_send_binn(worker->sock, C_OBJ_TARGETS, targets);
	binn_free(targets);
	return (SUCCESS);
}

int					handle_result_offer(t_mgr *mgr, t_wrkr *wrkr, uint8_t *msg)
{
	ssize_t			ret;
	binn			*obj;
	t_obj_hdr		*hdr;

	hdr = (t_obj_hdr *)msg;
	hdr->objlen = ntohl(hdr->objlen);
	if (hdr->objlen <= 0)
		return (FAILURE);
	if (!(obj = (binn *)memalloc(sizeof(uint8_t) * hdr->objlen)))
		return (hermes_error(FAILURE, "malloc() %s", strerror(errno)));
	ret = recv(wrkr->sock, obj, hdr->objlen, MSG_WAITALL);
	if (ret == 0)
	{
		free(obj);
		wrkr->stat.running = false;
		return (FAILURE);
	}
	if (hdr->code == C_OBJ_RES)
	{
		if (mgr->tpool)
			pthread_mutex_lock(&mgr->tpool->results_mtx);
		unbinnify_resultset(&mgr->results, &wrkr->targets, obj);
		if (mgr->tpool)
			pthread_mutex_unlock(&mgr->tpool->results_mtx);
	}
	free(obj);
	return (SUCCESS);
}

int					mgr_process_msg(t_mgr *mgr, t_wrkr *wrkr, uint8_t *msgbuff)
{
	t_msg_hdr		*hdr;

	if (!wrkr)
		return (FAILURE);
	hdr = (t_msg_hdr*)msgbuff;
	printf("type: %i code: %i\n", hdr->type, hdr->code);
	if (hdr->type == T_OBJ && hdr->code == C_OBJ_RES)
	{
		if (handle_result_offer(mgr, wrkr, msgbuff) == FAILURE)
			return (FAILURE);
	}
	return (SUCCESS);
}

void                poll_wrkr_msgs(t_mgr *mgr, nfds_t fditer,
                   struct pollfd *fds)
{
	uint8_t			msgbuff[PKT_SIZE];
	t_wrkr			**workers;
	ssize_t 		ret;

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
		while (fditer--)
		{
			if (fds[fditer].revents & POLLIN)
			{
				if (hermes_recvmsg(fds[fditer].fd, msgbuff) > 0)
				{
					if (workers[fds[fditer].fd])
						if (mgr_process_msg(mgr, workers[fds[fditer].fd],
						                    msgbuff) == FAILURE)
							return ;
				}
				else
				{
					if (fds[fditer].revents & POLLHUP)
					{
						printf("POLLHUP hit\n");
						close(fds[fditer].fd);
						fds[fditer].fd *= -1;
						mgr->workers.wrking_cnt--;
					}
				}
			}
		}
	}
}

void				add_wrkrtree_to_array(t_node *workers, t_wrkr **array)
{
	t_wrkr			*worker;

	if (!workers)
		return ;
	if (workers->left)
		add_wrkrtree_to_array(workers->left, array);
	worker = (t_wrkr*)workers->data;
	array[worker->sock] =  worker;
	if (workers->right)
		add_wrkrtree_to_array(workers->right, array);
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
			(*fds)[i].fd = -1;
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
	int				i;

	i = -1;
	pthread_mutex_lock(&mgr->tpool->amt_working_mtx);
	if (mgr->tpool->amt_working != mgr->tpool->thread_amt)
	{
		pthread_mutex_unlock(&mgr->tpool->amt_working_mtx);
		pthread_mutex_lock(&mgr->tpool->work_mtx);
		if (mgr->tpool->work->total == 0 && mgr->targets.total > 0)
		{
			transfer_work(&mgr->thread_work, &mgr->targets, mgr->tpool->reqest_amt);
			mgr->tpool->reqest_amt *= (mgr->tpool->reqest_amt < 2048) ? 2 : 1;
			tpool_event(mgr->tpool);
		}
		else if (mgr->targets.total == 0 && mgr->tpool->work->total == 0)
		{
			//TODO find out why threads aren't being switched off.
			while (++i < mgr->tpool->thread_amt)
			{
				if (mgr->tpool->threads[i].working == false)
				{
					mgr->tpool->threads[i].alive = false;
				}
			}
		}
		pthread_mutex_unlock(&mgr->tpool->work_mtx);
	}
	else
		pthread_mutex_unlock(&mgr->tpool->amt_working_mtx);
}

bool				mgr_finished(t_mgr *mgr)
{
	if (mgr->targets.total > 0 || mgr->workers.wrking_cnt != 0)
		return (false);
	if (mgr->tpool)
	{
		pthread_mutex_lock(&mgr->tpool->amt_alive_mtx);
		if (mgr->tpool->amt_alive != 0)
		{
			pthread_mutex_unlock(&mgr->tpool->amt_alive_mtx);
			return (false);
		}
		pthread_mutex_unlock(&mgr->tpool->amt_alive_mtx);
	}
	return (true);
}

void				check_results(t_mgr *mgr)
{
	t_ip4           *ip;
	t_result		*res;

	if (mgr->results.result_cnt > 0)
	{
		while (mgr->results.results)
		{
			res = (t_result*)mgr->results.results->data;
			if (res)
			{
				ip = new_ip4();
				memcpy(&ip->s_addr, &res->ip.s_addr, sizeof(t_ip4));
				printf("result: %d\n", ip->s_addr);
				list_rm_node(&mgr->results.results, &mgr->results.results, true);
				mgr->results.result_cnt--;
				if (rm_node_bst(&mgr->targets.ips, ip, ip4_cmp, ip4_min) == false)
				{
					if (rm_node_bst(&mgr->targets.iprngs, ip, ip4rng_cmp,
					                ip4rng_min) == false)
						return ;
					mgr->targets.rng_cnt--;
				}
				else
					mgr->targets.ip_cnt--;
				mgr->targets.total--;
			}
			res = mgr->results.results->data;
			printf("result: %s\n", inet_ntoa(res->ip));
			mgr->results.result_cnt--;
		}
	}
	if (mgr->tpool)
	{
		pthread_mutex_lock(&mgr->tpool->results_mtx);
		while (mgr->tpool->results->results)
		{
			list_add_head(&mgr->results.results,
			              &mgr->tpool->results->results->data);
			list_rm_node(&mgr->tpool->results->results,
			             &mgr->tpool->results->results, false);
			mgr->tpool->results->result_cnt -= 1;
			mgr->tpool->results->byte_size -= sizeof(mgr->tpool->results->byte_size);
			mgr->results.result_cnt += 1;
			mgr->results.byte_size += sizeof(mgr->results.results->data);
		}
		pthread_mutex_unlock(&mgr->tpool->results_mtx);
	}
}

void				send_workers_work(t_mgr *mgr)
{
	int         i;
	t_wrkr		**workers;

	workers = mgr->workers.wrkrs->data;
	for (i = 0; i < (int)mgr->workers.maxfd; i++)
	{
		if (workers[i])
		{
			if (workers[i]->targets.total == 0 && mgr->targets.total > 0)
			{
				printf("sending worker work\n");
				transfer_work(&workers[i]->targets, &mgr->targets, workers[i]->send_size);
				workers[i]->send_size *= 2;
				send_work(workers[i]);
				if (workers[i]->stat.has_work == false)
				{
					workers[i]->stat.has_work = true;
					mgr->workers.wrking_cnt += 1;
				}
			}
			else if (workers[i]->targets.total == 0 && mgr->targets.total == 0)
			{
				hermes_sendmsgf(workers[i]->sock, msg_tc(T_SHUTDOWN, C_SHUTDOWN_SFT), NULL);
				mgr->workers.wrking_cnt -= 1;
				mgr->workers.cnt -= 1;
				workers[i]->stat.has_work = false;
				free(workers[i]);
				workers[i] = NULL;
			}
		}
	}
}

void				flatten_portset(t_portset *set)
{
	t_prtrng		*rng;
	uint16_t		start;
	t_node			*tmp;
	size_t			i;

	i = 0;
	if (!(set->flat = memalloc(sizeof(uint16_t) * (set->total + 1))))
	{
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
		return;
	}
	tmp = set->ports;
	while (tmp)
	{
		set->flat[i++] = ((t_port*)tmp->data)->port;
		tmp = tmp->right;
	}
	tmp = set->prtrngs;
	while (tmp)
	{
		rng = tmp->data;
		start = rng->start;
		while (start <= rng->end)
			set->flat[i++] = start++;
		tmp = tmp->right;
	}
}

void				flatten_all_portsets(t_env *env)
{
	if (env->ports.total > 0)
		flatten_portset(&env->ports);
	if (env->ack_ports && env->ack_ports->total > 0)
	{
		flatten_portset(env->ack_ports);
		del_list(&env->ack_ports->ports, true);
		del_list(&env->ack_ports->prtrngs, true);
		free(env->ack_ports);
		env->ack_ports = NULL;
	}
	if (env->syn_ports && env->syn_ports->total > 0)
	{
		flatten_portset(env->syn_ports);
		del_list(&env->syn_ports->ports, true);
		del_list(&env->syn_ports->prtrngs, true);
		free(env->syn_ports);
		env->syn_ports = NULL;
	}
	if (env->udp_ports && env->udp_ports->total > 0)
	{
		flatten_portset(env->udp_ports);
		del_list(&env->udp_ports->ports, true);
		del_list(&env->udp_ports->prtrngs, true);
		free(env->udp_ports);
		env->udp_ports = NULL;
	}
}

uint16_t		*make_tcp_dstports(size_t size)
{
	uint16_t	range;
	uint16_t	*dstp;
	size_t		i;

	i = 0;
	srand((uint)time(NULL));
	range = EPHIM_MAX - EPHIM_MIN;
	if (!(dstp = memalloc(sizeof(uint16_t) * size)))
	{
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
		return (NULL);
	}
	while (i < size)
	{
		dstp[i++] = (uint16_t)(rand() % range + EPHIM_MIN);
	}
	i = 0;
	while (i < size)
	{
		printf("%u\n", dstp[i++]);
	}
	return (dstp);
}

int					manager_loop(t_mgr *mgr)
{
	struct pollfd	*fds;

	fds = NULL;
	memset(&mgr->results, 0, sizeof(t_resultset));
	mgr->stat.running = true;
	flatten_all_portsets(&mgr->env);
	mgr->env.dstports = make_tcp_dstports(mgr->env.ports.total);
	if (mgr->env.opts.thread_count > 0)
	{
		if (!(mgr->tpool = init_threadpool(&mgr->env, &mgr->thread_work, &mgr->results)))
			hermes_error(EXIT_FAILURE, "init_threadpool()");
	}
	if (mgr->workers.cnt > 0)
		init_workers(mgr, &fds);
	while (mgr->stat.running == true)
	{
		if (mgr->workers.cnt > 0)
		{
			poll_wrkr_msgs(mgr, mgr->workers.maxfd, fds);
			send_workers_work(mgr);
		}
		if (mgr->tpool)
			tend_threads(mgr);
		check_results(mgr);
		if (mgr_finished(mgr) == true)
			mgr->stat.running = false;
	}
	if (mgr->tpool)
		tpool_kill(&mgr->tpool);
	free(fds);
	return (SUCCESS);
}
