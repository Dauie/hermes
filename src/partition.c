#include "../incl/hermes.h"

// TODO : function pointers to cmp, min in struct?
void partition_ip4(t_node **src, t_node **w_jobs, int (*cmp)(void *, void *), void *(*min)(t_node *))
{
    if (!w_jobs)
        return ;
    if ((*w_jobs)->left)
        partition_ip4(src, &(*w_jobs)->left, cmp, min);
    add_node_bst(&((t_job*)((t_node*)w_jobs)->data)->targets.ips, &(*src)->data, cmp);
    remove_node_bst(src, (*src)->data, cmp, min);
    if ((*w_jobs)->right)
        partition_ip4(src, &(*w_jobs)->right, cmp, min);
}

void partition_port(t_node **src, t_node **w_jobs, int (*cmp)(void *, void *), void *(*min)(t_node *))
{
    if (!w_jobs)
        return ;
    if ((*w_jobs)->left)
        partition_ip4(src, &(*w_jobs)->left, cmp, min);
    add_node_bst(&((t_job*)((t_node*)w_jobs)->data)->ports.ports, &(*src)->data, cmp);
    remove_node_bst(src, (*src)->data, cmp, min);
    if ((*w_jobs)->right)
        partition_ip4(src, &(*w_jobs)->right, cmp, min);
}

void partition_ip4rng(uint32_t parts, t_node **src, t_node **w_jobs, int (*cmp)(void *, void *), void *(*min)(t_node *))
{
    t_node *temp;
    t_node *partitioned;

    if (!w_jobs)
        return ;
    temp = NULL;
    partitioned = split_nip4rng(parts, (*src)->data);
    while (partitioned)
    {
        if (!temp)
            temp = *w_jobs;
        add_node_bst(&((t_job*)((t_node*)w_jobs)->data)->targets.iprngs, &partitioned->data, cmp);
        partitioned = remove_node_list_head(&partitioned);
        temp = temp->right;
    }
    remove_node_bst(src, (*src)->data, cmp, min);
}

void partition_portrng(uint32_t parts, t_node **src, t_node **w_jobs, int (*cmp)(void *, void *), void *(*min)(t_node *))
{
    t_node *temp;
    t_node *partitioned;

    if (!w_jobs)
        return ;
    temp = NULL;
    // NOTE : this /should/ work
    partitioned = split_nprtrng(parts, &src);
    while (partitioned)
    {
        if (!temp)
            temp = *w_jobs;
        add_node_list_head(&((t_job*)((t_node*)w_jobs)->data)->ports.prtrngs, &partitioned->data);
        partitioned = remove_node_list_head(&partitioned);
        temp = temp->right;
    }
    remove_node_bst(src, (*src)->data, cmp, min);
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

t_node  *partition_jobs(t_job *job, uint32_t worker_count)
{
    t_node      *job_list;

    job_list = new_joblist(&job->options, worker_count);
    while (job->targets.ips)
        partition_ip4(&job->targets.ips,
                      &job_list, ip4_cmp, ip4_min);
    while (job->ports.ports)
        partition_port(&job->ports.ports,
                       &job_list, port_cmp, port_min);
    while (job->targets.iprngs)
        partition_ip4rng(worker_count,
                         &job->targets.iprngs,
                         &job_list, ip4rng_cmp, ip4rng_min);
    while (job->ports.prtrngs)
        partition_portrng(worker_count,
                          &job->ports.prtrngs,
                          &job_list, portrng_cmp, portrng_min);
    return (job_list);
}
