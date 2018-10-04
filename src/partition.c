#include "../incl/hermes.h"

void partition_ip4(t_node **src, t_node **w_jobs, int (*cmp)(void *, void *))
{
    if (!w_jobs)
        return ;
    if (w_jobs.left)
        partition_ip4(src, &w_jobs.left, cmp);
    add_node(&w_jobs->data->targets->ip4, &src->data, cmp);
    remove_node(src, src->data, cmp);
    if (w_jobs.right)
        partition_ip4(src, &w_jobs.right, cmp);
}

void partition_port(t_node **src, t_node **w_jobs, int (*cmp)(void *, void *))
{
    if (!w_jobs)
        return ;
    if (w_jobs->left)
        partition_port(src, &w_jobs->left, cmp);
    add_node(&w_jobs->data->scan_portlist->ports, &src->data, cmp);
    remove_node(src, src->data, cmp);
    if (w_jobs.right)
        partition_port(src, &w_jobs.right, cmp);
}

t_node *split_tree(t_node **src, int threads)
{
    t_node *tree;

    return (tree);
}

void partition_ip4rng(uint32_t parts, t_node **src, t_node **w_jobs)
{
    t_node *temp;
    t_node *partitioned;

    if (!w_jobs)
        return ;
    temp = NULL;
    // NOTE : this /should/ work
    partitioned = split_nip4rng(parts, *src->data, ip4rng_cmp);
    while (partitioned)
    {
        if (!temp)
            temp = *w_jobs;
        add_node(&w_jobs->data->targets->iprange, &partitioned->data, ip4rng_cmp, ip4rng_min);
        remove_node(&partitioned, partitioned->data, ip4rng_cmp, ip4rng_min);
        temp = temp->next;
    }
    remove_node(&src, src->data, cmp);
}

void partition_portrng(uint32_t parts, t_node **src, t_node **w_jobs, int (*cmp)(void *, void *))
{
    t_node *temp;
    t_node *partitioned;

    if (!w_jobs)
        return ;
    temp = NULL;
    // NOTE : this /should/ work
    partitioned = split_nprtrng(parts, &src, parts);
    while (partitioned)
    {
        if (!temp)
            temp = w_jobs;
        add_node(&w_jobs->data->ports->port_range, &partitioned->data, cmp);
        remove_node(&partitioned, partitioned->data, cmp);
        temp = temp->next;
    }
    remove_node(&src, src->data, cmp);
}

int     job_cmp(void *lft, void *rgt)
{
    return (-1);
}

t_node *new_jobtree(uint32_t wrkr_cnt)
{
    uint32_t total;

    total = 0;
    if (!(job_tree = (t_node*)memalloc(sizeof(t_node))))
        // hermes error
        ;
    if (!(job_tree->data = (t_job*)memalloc(sizeof(t_job))))
        // hermes error
        ;
    if (!(job_tree->data->options = (t_ops*)memalloc(sizeof(t_ops))))
        ;
    if (!(job_tree->data->targets= (t_targetlist*)memalloc(sizeof(t_targetlist))))
        ;
    job_tree->data->ports;
    job_tree->data->syn_ports;
    job_tree->data->ack_ports;
    job_tree->data->udp_ports;
    while (total < worker_count)
    {
        add_node(&job_tree, job_tree->data, job_cmp);
        total++;
    }
}

t_node  *partition_jobs(t_job *job, uint32_t worker_count)
{
    uint32_t    total;
    t_node      *job_tree;

    job_tree = new_jobtree(worker_count);
    while (job->targets->ip)
        partition_ip4(&job->targets->ip,
                      &job_tree, ip4_cmp);
    while (job->ports.ports)
        partition_port(&job->ports.ports,
                       &job_tree, port_cmp);
    while (job->targets->iprange)
        partition_ip4rng(worker_count,
                         &job->targets->iprange,
                         &job_tree, ip4rng_cmp);
    while (job->ports.prtrngs)
        partition_portrng(worker_count,
                          &job->ports.prtrngs,
                          &job_tree, portrng_cmp);
    return (job_tree);
}
