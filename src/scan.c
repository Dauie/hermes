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
			list_add_head(&res_ptr->results, (void**)&result);
			res_ptr->result_cnt += 1;
			res_ptr->byte_size += sizeof(result);
			pthread_mutex_unlock(res_mtx);
			if (list_rm_node(&targets->ips, &targets->ips, true) == false)
				break;
			targets->total--;
			targets->ip_cnt--;
		}
		while (targets->iprngs)
		{
			curr = (t_ip4rng*)targets->iprngs->data;
			while (ntohl(curr->start) <= ntohl(curr->end))
			{
				sleep(1);
				if (!(result = (t_result*)memalloc(sizeof(t_result))))
					return;
				result->ip.s_addr = curr->start;
				pthread_mutex_lock(res_mtx);
				list_add_head(&res_ptr->results, (void **) &result);
				res_ptr->result_cnt += 1;
				res_ptr->byte_size += sizeof(result);
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

int						make_rx_filter(t_thread *thread)
{
	char				bpfexp[4096] = {0};
	int					i;
	int					x;

	i = -1;
	x = 0;
	while (thread->results[++i])
	{
		printf("adding %s to filter\n", inet_ntoa(thread->results[i]->ip));
		x += sprintf(&bpfexp[x], "src host %s ", inet_ntoa(thread->results[i]->ip));
		if (thread->results[i + 1])
			x += sprintf(&bpfexp[x], "and ");
	}
	if (pcap_compile(thread->pcaphand, &thread->filter, bpfexp, 0, PCAP_NETMASK_UNKNOWN) == -1)
		return (hermes_error(FAILURE, "pcap_compile() %s", pcap_geterr(thread->pcaphand)));
	if (pcap_setfilter(thread->pcaphand, &thread->filter) == -1)
		return (hermes_error(FAILURE, "pcap_setfilter() %s", pcap_geterr(thread->pcaphand)));
	return (SUCCESS);
}

void				inflate_targetset_into_results(t_targetset *set, t_thread *thread, t_env *env)
{
	int				i;
	t_ip4rng		*rng;
	t_ip4			start;
	t_node			*tmp;

	i = 0;
	tmp = set->ips;
	while (tmp)
	{
		thread->results[i] = new_result();
		thread->results[i]->ip.s_addr = ((t_ip4*)tmp->data)->s_addr;
		printf("thread adding %s to thread's results\n", inet_ntoa(thread->results[i]->ip));
		thread->results[i]->portstats = memalloc(sizeof(t_portstat *) * (env->ports.total + 1));
		i++;
		tmp = tmp->right;
	}
	del_list(&set->ips, true);
	tmp = set->iprngs;
	while (tmp)
	{
		rng = tmp->data;
		start.s_addr = rng->start;
		while (ip4_cmp(&start, &rng->end) <= 0)
		{
			thread->results[i] = new_result();
			thread->results[i]->ip.s_addr = start.s_addr;
			printf("thread adding %s to thread's results\n", inet_ntoa(thread->results[i]->ip));
			thread->results[i]->portstats = memalloc(sizeof(t_portstat *) * (env->ports.total + 1));
			i++;
			start.s_addr = ip4_increment(start.s_addr, 1);
		}
		tmp = tmp->right;
	}
	del_list(&set->iprngs, true);
}

void add_results_to_lookup(t_thread *thread, size_t count)
{
	size_t			i;

	i = 0;
	while (i < count)
	{
		hashtbl_add(thread->lookup, (uint8_t *)&thread->results[i]->ip, 4, (void **)&thread->results[i]);
		i++;
	}
	printf("add %zu elements to lookup\n", i);
}

void				run_scan(t_thread *thread, t_targetset *set)
{
	t_result		*tmp;

	inflate_targetset_into_results(set, thread, thread->pool->env);
	add_results_to_lookup(thread, set->total);
	make_rx_filter(thread);



	for (size_t i = 0; i < set->total; i++)
	{
		if (hashtbl_get(thread->lookup, (uint8_t *)&thread->results[i]->ip, 4, (void **)&tmp) == true)
			printf("thread %d found result %zu %s\n", thread->id, i, inet_ntoa(tmp->ip));
	}
	for (size_t i = 0; i < set->total; i++)
	{
		printf("thread %d deleting %s ...", thread->id, inet_ntoa(thread->results[i]->ip));
		if (hashtbl_rm(thread->lookup, (uint8_t *)&thread->results[i]->ip, 4) == true)
			printf("done\n");
		else
			printf("failed\n");
	}
}
