#include "../incl/hermes.h"

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
			sleep(1);
			if (!(result = new_result()))
				return ;
			memcpy(&result->ip, (void*)(t_ip4*)targets->ips->data, sizeof(t_ip4));
			pthread_mutex_lock(res_mtx);
			list_add_head(&res_ptr->results, (void**)&result);
			res_ptr->result_cnt += 1;
			res_ptr->byte_size += sizeof(result);
			pthread_mutex_unlock(res_mtx);
			if (list_rm_node(&targets->ips, false) == false)
				break;
			targets->total--;
			targets->ip_cnt--;
		}
		while (targets->iprngs)
		{
			curr = (t_ip4rng*)targets->iprngs->data;
			while (ntohl(curr->start) <= ntohl(curr->end))
			{
				if (!(result = (t_result*)memalloc(sizeof(t_result))))
					return;
				memcpy(&result->ip, &curr->start, sizeof(t_ip4));
				pthread_mutex_lock(res_mtx);
				list_add_head(&res_ptr->results, (void **) &result);
				res_ptr->result_cnt += 1;
				res_ptr->byte_size += sizeof(result);
				pthread_mutex_unlock(res_mtx);
				curr->start = ip4_increment(curr->start, 1);
				targets->total--;
			}
			if (list_rm_node(&targets->iprngs, false) == false)
				break;
			targets->rng_cnt--;
		}
	}
}