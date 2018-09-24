#include "../incl/defined.h"
#include "../incl/hermes.h"

#ifndef WORKER
#define WORKER(w) ((t_worker*)w->data)
#endif

void    set_hints(struct addrinfo *hints)
{
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = 0;
    hints->ai_protocol = 0;
}

int     connect_workers(t_node *w, size_t n)
{
    char                ip[INET_ADDRSTRLEN];
    struct addrinfo     *res;
    struct addrinfo     hints;

    if (!w)
        return (0);
    res = NULL;
    memset(&hints, 0, sizeof(struct addrinfo));
    set_hints(&hints);
    if (n > 0 && WORKER(w))
    {
        // TODO : itoa hermeslib
        // not sure if want to return error or keep trying for all workers
        if (inet_ntop(AF_INET, &WORKER(w)->ip, ip, sizeof(ip)) > 0)
            if (getaddrinfo(ip, itoa(WORKER(w)->port), &hints, &res) >= 0)
                if ((WORKER(w)->sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) >= 0)
                    if (connect(WORKER(w)->sock, res->ai_addr, res->ai_addrlen) >= 0)
                        if (bind(WORKER(w)->sock, res->ai_addr, res->ai_addrlen) < 0)
                            close(WORKER(w)->sock);
        freeaddrinfo(res);
        if (w->left)
            return (connect_workers(w->left, n - 1));
        if (w->right)
            return (connect_workers(w->right, n - 1));
    }
    return (SUCCESS);
}

void    manager(t_job *job)
{
    if (connect_workers(job->worker_list.workers, job->worker_list.worker_count) < 0)
        hermes_error(INPUT_ERROR, TRUE, 2, "connect_workers()", strerror(errno));
    // TODO : do things
}

#ifdef TESTING
void    prompt(char *output, char *input)
{
    printf("%s", output);
    fgets(input, (int)strlen(input), stdin);
}

int main(void)
{
    uint32_t    *ip;
    t_job       *job;
    t_node      *worker;
    char        input[20];

    job = NULL;
    while (TRUE)
    {
        ip = NULL;
        prompt("\n> ", input);
        if (!memcmp("connect", input, 7))
        {
            manager(job);
        }
        else if (!memcmp("add", input, 3))
        {
            worker = new_node();
            worker->data = new_worker();
            if (add_node(&job->worker_list.workers, &worker, worker_cmp) < 0)
                hermes_error(INPUT_ERROR, TRUE, 2, "adding worker", strerror(errno));
            prompt("\nip > ", input);
            if (parse_ip(&WORKER(worker)->ip, input) < 0)
                hermes_error(INPUT_ERROR, TRUE, 2, "parsing ip", strerror(errno));
            prompt("\nport > ", input);
            if (parse_port(&WORKER(worker)->port, input) < 0)
                hermes_error(INPUT_ERROR, TRUE, 2, "parsing port", strerror(errno));
        }
        else if (!memcmp("del", input, 3))
        {
            prompt("\nip > ", input);
            if (parse_ip(ip, input) < 0)
                hermes_error(INPUT_ERROR, TRUE, 2, "parsing ip", strerror(errno));
            if (remove_node(&job->worker_list.workers, ip, worker_cmp, worker_min) < 0)
                hermes_error(INPUT_ERROR, TRUE, 2, "removing worker", strerror(errno));
        }
        else if (memcmp("quit", input, 4) == 0 ||
                 memcmp("exit", input, 4) == 0)
        {
            break;
        }
        else
        {
            printf("command not recognized");
        }
        fflush(stdin);
    }
}
#endif