#include "../incl/defined.h"
#include "../incl/hermes.h"

#ifndef WORKER
#define WORKER(w) ((t_worker*)w->data)
#endif

int				connect_workers(t_node **workers, uint32_t *worker_count,
                                   t_node **rm_tree, int proto)
{
	t_worker		*worker;

	if (!*workers)
		return (0);
	if ((*workers)->left)
		connect_workers(&(*workers)->left, rm_tree, proto);
	worker = (*workers)->data;
	if ((worker->sock = socket(PF_INET, SOCK_STREAM, proto)) == -1)
		hermes_error(errno, TRUE, 2, "socket()", strerror(errno));
	if (connect(worker->sock, (const struct sockaddr *)&worker->sin,
				sizeof(worker->sin)) == -1)
	{
		hermes_error(FAILURE, FALSE, 2, "could not connect to worker:", inet_ntoa(worker->sin.sin_addr));
		remove_node(rm_tree, worker, worker_cmp, worker_min);
		*worker_count -= 1;
	}
	else
		printf("connected to %s.\n", inet_ntoa(worker->sin.sin_addr));
	if ((*workers)->right)
		connect_workers(&(*workers)->right, rm_tree, proto);
}

void    distribute_jobs(t_node *worker)
{
    if (!worker)
        return ;
    send_work(worker);
    distribute_jobs(worker->left);
    distribute_jobs(worker->right);
}

int					manager(t_mgr *mgr)
{
	struct protoent	*proto;

	if ((proto = getprotobyname("tcp")) == 0)
		return (FAILURE);
	if (mgr->worker_list.wrkr_cnt > 0)
	{
		connect_workers(&mgr->worker_list.wrkrs, &mgr->worker_list.wrkr_cnt,
						&mgr->worker_list.wrkrs, proto->p_proto);
		printf("connected to %zu wrkrs.\n", mgr->worker_list.wrkr_cnt);
        partition_jobs(mgr->job, mgr->worker_list);
        distribute_jobs(mgr->worker_list.wrkrs);
	}
    /* TODO: Divide work amongst thread count, send jobs to wrkrs, spawn threads
    partition_jobs(mgr->job, mgr->thread_count);
    distribute_jobs(mgr->threads);
    */
    run_hermes(mgr);
	return (0);
}

#ifdef TESTING
void    prompt(char *output, char *input, int buflen)
{
	printf("%s", output);
	fgets(input, buflen, stdin);
}

static int	skip(int size, const char *s)
{
	while (s[size] == ' ' || s[size] == '\n' ||
			s[size] == '\t' || s[size] == '\0')
		size--;
	return (size);
}

char		*strtrim(const char *s)
{
	int		size;
	char	*ret;

	if (!s)
		return (0);
	while (*s == ' ' || *s == '\n' || *s == '\t' || *s == '\0' || *s == '\r')
	{
		if (s == 0 || *s == 0)
		{
			if (!(ret = (char*)memalloc(sizeof(char))))
				return (0);
			*ret = 0;
			return (ret);
		}
		s++;
	}
	size = (strlen(s));
	size = skip(size, s);
	size++;
	if (!(ret = (char*)malloc(sizeof(char) * (size + 1))))
		return (0);
	strncpy(ret, s, size);
	ret[size] = 0;
	return (ret);
	free(ret);
}

int main(void)
{
	uint32_t    ips;
	t_job       *job;
	t_node      *worker;
	char        input[20];

	ips = NULL;
	job = (t_job*)memalloc(sizeof(t_job));
	job->worker_list = *(t_workerlist*)memalloc(sizeof(t_workerlist));
	while (TRUE)
	{
		prompt("> ", input, 20);
		if (!memcmp("connect", input, 7))
		{
			manager(job);
		}
		else if (!memcmp("add", input, 3))
		{
			worker = new_node();
			worker->data = new_worker();
			prompt("ips > ", input, 20);
			if (parse_ip(&WORKER(worker)->ips, strtrim(input)) < 0)
				hermes_error(INPUT_ERROR, TRUE, 2, "parsing ips", strerror(errno));
			prompt("ports > ", input, 20);
			if (parse_port(&WORKER(worker)->ports, strtrim(input)) < 0)
				hermes_error(INPUT_ERROR, TRUE, 2, "parsing ports", strerror(errno));
			if (add_node(&(job->worker_list.wrkrs), &worker, worker_cmp) < 0)
				hermes_error(INPUT_ERROR, TRUE, 2, "adding worker", strerror(errno));
			job->worker_list.wrkr_cnt++;
		}
		else if (!memcmp("del", input, 3))
		{
			prompt("ips > ", input, 20);
			if (parse_ip(&ips, strtrim(input)) < 0)
				hermes_error(INPUT_ERROR, TRUE, 2, "parsing ips", strerror(errno));
			if (remove_node(&job->worker_list.wrkrs, ips, worker_cmp, worker_min) < 0)
				hermes_error(INPUT_ERROR, TRUE, 2, "removing worker", strerror(errno));
			job->worker_list.wrkr_cnt--;
		}
		else if (!memcmp("quit", input, 4) ||
				!memcmp("exit", input, 4))
		{
			break;
		}
		fflush(stdin);
	}
}
#endif