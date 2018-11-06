#include "../incl/hermes.h"

void				test_run_scan(t_env *env, t_targetset *targets, t_resultset *res_ptr, pthread_mutex_t *res_mtx)
{
	t_result	*result;
	t_ip4rng	*curr;

	if (!env || !targets || !res_ptr)
		return ;
	if (targets->total > 0)
	{
		while (targets->ips)
		{
			sleep(1);
			if (!(result = new_result()))
				return ;
			result->ip.s_addr = ((t_ip4 *)targets->ips->data)->s_addr;
			pthread_mutex_lock(res_mtx);
			list_add_head(&res_ptr->results, (void **)&result);
			res_ptr->result_cnt += 1;
			pthread_mutex_unlock(res_mtx);
			if (list_rm_node(&targets->ips, &targets->ips, true) == false)
				break;
			targets->total--;
			targets->ip_cnt--;
		}
		while (targets->iprngs)
		{
			curr = (t_ip4rng *)targets->iprngs->data;
			while (ntohl(curr->start) <= ntohl(curr->end))
			{
				sleep(1);
				if (!(result = (t_result *)memalloc(sizeof(t_result))))
					return;
				result->ip.s_addr = curr->start;
				pthread_mutex_lock(res_mtx);
				list_add_head(&res_ptr->results, (void **) &result);
				res_ptr->result_cnt += 1;
				pthread_mutex_unlock(res_mtx);
				curr->start = ip4_increment(curr->start, 1);
				targets->total--;
			}
			if (list_rm_node(&targets->iprngs, &targets->iprngs, true) == false)
				break;
			targets->rng_cnt--;
		}
	}
}

void				run_scan(t_thread *thread, t_targetset *targets)
{
	(void)thread;
	(void)targets;
}
