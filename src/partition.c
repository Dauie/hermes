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

void partition_port(t_node **src, t_node *w_jobs)
{
	if (!w_jobs)
		return ;
	if (w_jobs->left)
		partition_port(src, w_jobs->left);
	add_node_bst(&((t_job*)w_jobs->data)->ports.ports, &(*src)->data, port_cmp);
	remove_node_bst(src, (*src)->data, port_cmp, port_min);
	if (w_jobs->right)
		partition_port(src, w_jobs->right);
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

void partition_portrng(uint32_t parts, t_node **src, t_node *w_jobs)
{
	t_node *tmp;
	t_node *fragment_list;

	if (!w_jobs)
		return ;
	tmp = NULL;
	// NOTE : this /should/ work
	fragment_list = split_prtrng_n(&src, parts);
	while (fragment_list)
	{
		if (!tmp)
			tmp = w_jobs;
		add_node_list_head(&((t_job*)((t_node*)w_jobs)->data)->ports.prtrngs, &fragment_list->data);
		remove_node_list_head(&fragment_list);
		tmp = tmp->right;
	}
	remove_node_bst(src, (*src)->data, port_cmp, port_min);
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
		add_node_list_head(&job, tmp);
		count--;
		free(tmp);
	}
	return (job);
}

t_node  *partition_jobs(t_job *job, uint32_t parts)
{
	t_node      *job_list;

	job_list = new_joblist(&job->options, parts);
	while (job->targets.ips)
		partition_ip4(&job->targets.ips, job_list);
	while (job->ports.ports)
		partition_port(&job->ports.ports, job_list);
	while (job->targets.iprngs)
		partition_ip4rng(parts, &job->targets.iprngs, job_list);
	while (job->ports.prtrngs)
		partition_portrng(parts, &job->ports.prtrngs, job_list);
	return (job_list);
}
