#include "../incl/hermes.h"
#include "../incl/defined.h"

int					connect_workers(t_node **workers, uint32_t *worker_count,
						t_node **rm_tree, int proto)
{
	t_wrkr		*worker;

	if (!*workers)
		return (0);
	if ((*workers)->left)
		connect_workers(&(*workers)->left, worker_count, rm_tree, proto);
	worker = (t_wrkr*)(*workers)->data;
	if ((worker->sock = socket(PF_INET, SOCK_STREAM, proto)) == -1)
		return (hermes_error(FAILURE, 2, "socket()", strerror(errno)));
	if (connect(worker->sock, (const struct sockaddr *)&worker->sin,
				sizeof(worker->sin)) == -1)
	{
		hermes_error(FAILURE, 2, "could not connect to worker. dropping:", inet_ntoa(worker->sin.sin_addr));
		if (remove_node_bst(rm_tree, worker, worker_cmp, worker_min) == true)
			*worker_count -= 1;
	}
	else
		printf("connected to %s.\n", inet_ntoa(worker->sin.sin_addr));
	if ((*workers)->right)
		connect_workers(&(*workers)->right, worker_count, rm_tree, proto);
	return (0);
}

/* TODO if job is rejected by worker for some reason, it should be divided again and sent to other workers */

void				assign_targetsets(t_node *wrkr_tree, t_node **job_lst)
{

	if (!wrkr_tree || !*job_lst)
		return ;
	if (wrkr_tree->left)
		assign_targetsets(wrkr_tree->left, job_lst);
	wrkr_tree->data = (*job_lst)->data;
//	if (send_work(wrkr_tree->data, (t_job*)(*job_lst)->data) == FAILURE)
//		;
	remove_list_head(job_lst, false);
	if (wrkr_tree->right)
		assign_targetsets(wrkr_tree->right, job_lst);
}

/* TODO manager is left without work atm */
int					manager_loop(t_msession *mgr)
{
	struct protoent	*proto;
	t_node			*targetset_list;

	if ((proto = getprotobyname("tcp")) == 0)
		return (FAILURE);
	if (mgr->workers && mgr->workers->wrkr_cnt > 0)
	{
		connect_workers(&mgr->workers->wrkrs, &mgr->workers->wrkr_cnt,
				&mgr->workers->wrkrs, proto->p_proto);
		printf("connected to %i workers.\n", mgr->workers->wrkr_cnt);
		printf("partitioning work...\n");
		/* TODO remove targetset from mgr, and replace with one from list, then distribute targetsets to workers */
		targetset_list = partition_targetset(mgr->job.targets, mgr->workers->wrkr_cnt + 1);
		printf("done.\nsending work...\n");
		assign_targetsets(mgr->workers->wrkrs, &targetset_list);
		/* TODO track all memory used up to this point and refactor accordingly */
	}
	targetset_list = partition_targetset(mgr->job.targets, mgr->job.opts.thread_count);
	assign_targetsets(mgr->workers->wrkrs, &targetset_list);
	//run_hermes(mgr);
	del_list(&targetset_list, 0);
	return (0);
}

