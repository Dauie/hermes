//
// Created by Samuel Collet on 10/3/18.
//

//
// Created by Samuel Collet on 10/3/18.
//

void partition_ip4(t_node **src, t_node **w_jobs, int (*cmp)(void *, void *))
{
    if (!w_jobs)
        return ;
    if (w_jobs.left)
        partition_ip4(src, &w_jobs.left, cmp);
    add_node(&w_jobs->targets->ip4, &src->data, cmp);
    remove_node(src, src->data, cmp);
    if (w_jobs.right)
        partition_ip4(src, &w_jobs.right, cmp);
}

void partition_port(t_node **src, t_node **w_jobs, int (*cmp)(void *, void *))
{
    if (!w_jobs)
        return ;
    if (w_jobs.left)
        partition_port(src, &w_jobs.left, cmp);
    add_node(&w_jobs->scan_portlist->ports, &src->data, cmp);
    remove_node(src, src->data, cmp);
    if (w_jobs.right)
        partition_port(src, &w_jobs.right, cmp);
}

t_node *split_tree(t_node **src, int threads)
{
    t_node *tree;

    return (tree);
}

void partition_ip4rng(uint32_t parts, t_node **src, t_node **w_jobs, int (*cmp)(void *, void *))
{
    t_node *temp;
    t_node *partitioned;

    if (!w_jobs)
        return ;
    temp = NULL;
    // TODO balanced split over number of nodes
    partitioned = *split_nip4rng(parts, *src->data, ip4rng_cmp);
    while (partitioned)
    {
        if (!temp)
            temp = w_jobs;
        add_node(&w_jobs->targets->iprange, &partitioned->data, cmp);
        remove_node(&partitioned, partitioned->data, cmp);
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
    // TODO balanced split over number of nodes
    partitioned = split_split_nprtrng(parts, &src, num_threads);
    while (partitioned)
    {
        if (!temp)
            temp = w_jobs;
        add_node(&w_jobs->scan_portlist->port_range, &partitioned->data, cmp);
        remove_node(&partitioned, partitioned->data, cmp);
        temp = temp->next;
    }
    remove_node(&src, src->data, cmp);
}

void	partition_work(t_job *job, t_workerlist *worker_list)
{
    while (job->targets->ip)
        partition_ip4(&job->targets->ip,
                      &w_jobs, ip4_cmp);

    while (job->scan_portlist.ports)
        partition_port(&job->scan_portlist.ports,
                       &w_jobs, port_cmp);

    while (job->targets->iprange)
        partition_ip4rng(worker_list->wrkr_count,
                         &job->targets->iprange,
                         &w_jobs, ip4rng_cmp);

    while (job->scan_portlist.port_range)
        partition_portrng(worker_list->wrkr_count,
                          &job->scan_portlist.port_range,
                          &w_jobs, portrng_cmp);
}
