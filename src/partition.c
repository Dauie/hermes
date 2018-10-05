#include "../incl/hermes.h"

// TODO : function pointers to cmp, min in struct?
void partition_ip4(t_node **src, t_node *w_jobs)
{
	if (!w_jobs)
		return ;
	if (w_jobs->left)
		partition_ip4(src, w_jobs->left);
	add_node_bst(&((t_job*)w_jobs->data)->targets.ips, &(*src)->data, ip4_cmp);
	remove_node_bst(src, (*src)->data, ip4_cmp, ip4_min);
	if (w_jobs->right)
		partition_ip4(src, w_jobs->right);
}

void partition_ip4rng(uint32_t parts, t_node **src, t_node *w_jobs)
{
	t_node	*fragment_lst;
	t_node	*tmp;

	if (!w_jobs)
		return ;
	tmp = w_jobs;
	fragment_lst = split_ip4rng_n((*src)->data, parts);
	while (fragment_lst)
	{
		if (!tmp)
			tmp = w_jobs;
		add_node_bst(&((t_job*)w_jobs->data)->targets.iprngs, &fragment_lst->data, ip4rng_cmp);
		remove_node_list_head(&fragment_lst);
		tmp = tmp->right;
	}
	remove_node_bst(src, (*src)->data, ip4rng_cmp, ip4rng_min);
}

t_node *new_joblist(t_ops *ops, uint32_t count)
{
	t_job       *tmp;
	t_node      *job;

	job = NULL;
	while (count)
	{
		tmp = new_job();
		memcpy(&tmp->options, ops, sizeof(t_ops));
		add_node_list_head(&job, (void **)&tmp);
		count--;
	}
	return (job);
}

t_node  *partition_jobs(t_job *job, uint32_t parts)
{
	t_node      *job_list;

	if (!parts)
		return (NULL);
	job_list = new_joblist(&job->options, parts);
	while (job->targets.ips)
		partition_ip4(&job->targets.ips, job_list);
	while (job->targets.iprngs)
		partition_ip4rng(parts, &job->targets.iprngs, job_list);
	return (job_list);
}
