#include "../incl/hermes.h"
#include "../incl/defined.h"

int					connect_workers(t_node **workers, uint32_t *worker_count,
						t_node **rm_tree, int proto)
{
	t_worker		*worker;

	if (!*workers)
		return (0);
	if ((*workers)->left)
		connect_workers(&(*workers)->left, worker_count, rm_tree, proto);
	worker = (t_worker*)(*workers)->data;
	if ((worker->sock = socket(PF_INET, SOCK_STREAM, proto)) == -1)
		hermes_error(errno, TRUE, 2, "socket()", strerror(errno));
	if (connect(worker->sock, (const struct sockaddr *)&worker->sin,
				sizeof(worker->sin)) == -1)
	{
		hermes_error(FAILURE, FALSE, 2,
				"could not connect to worker dropping:",
				inet_ntoa(worker->sin.sin_addr));
		remove_node_bst(rm_tree, worker, worker_cmp, worker_min);
		*worker_count -= 1;
	}
	else
		printf("connected to %s.\n", inet_ntoa(worker->sin.sin_addr));
	if ((*workers)->right)
		connect_workers(&(*workers)->right, worker_count, rm_tree, proto);
	return (0);
}

/* TODO if job is rejected by worker for some reason, it should be divided again and sent to other worker_set */

void				assign_targetsets_to_workers(t_node *wrkr_tree, t_node *job_lst)
{
	t_node			*wrkrs;
	t_node			*jobs;


}

/* TODO manager is left without work atm */
int					manager_loop(t_mgr *mgr)
{
	struct protoent	*proto;
	t_node			*targetset_list;

	if ((proto = getprotobyname("tcp")) == 0)
		return (FAILURE);
	if (mgr->worker_set.wrkr_cnt > 0)
	{
		connect_workers(&mgr->worker_set.wrkrs, &mgr->worker_set.wrkr_cnt,
				&mgr->worker_set.wrkrs, proto->p_proto);
		printf("connected to %i worker_set.\n", mgr->worker_set.wrkr_cnt);
		printf("partitioning work...\n");
		/* TODO remove targetset from mgr, and replace with one from list, then distribute targetsets to workers*/
		targetset_list = partition_targetset(&mgr->job.targets, mgr->worker_set.wrkr_cnt + 1);
		printf("done.\nsending work...\n");
		assign_targetsets_to_workers(mgr->worker_set.wrkrs, targetset_list);
		del_list(&targetset_list);
	}
	targetset_list = partition_targetset(&mgr->job.targets, mgr->job.opts.thread_count);
	assign_targetsets_to_workers(mgr->worker_set.wrkrs, targetset_list);
	//run_hermes(mgr);
	del_list(&targetset_list);
	return (0);
}

#ifdef TESTING
#ifndef WORKER
#define WORKER(w) ((t_worker*)w->data)
#endif

#ifndef IP4
#define IP4(m) (((t_ip4*)mgr->job.targets.ips->data)->s_addr)
#endif

#ifndef PORT
#define PORT(m) (((t_port*)mgr->job.ports.ports->data)->port)
#endif

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
	t_mgr		*mgr;
	t_node      *worker;
	char        input[20];

	if (!(mgr = (t_mgr *)memalloc(sizeof(t_mgr))))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	while (TRUE)
	{
		prompt("> ", input, 20);
		if (!memcmp("connect", input, 7))
		{
			manager(mgr);
		}
		else if (!memcmp("add", input, 3))
		{
			worker = new_node(new_worker());
			prompt("ips > ", input, 20);
			if (parse_ip(&IP4(mgr), strtrim(input)) < 0)
				hermes_error(INPUT_ERROR, TRUE, 2, "parsing ips", strerror(errno));
			prompt("ports > ", input, 20);
			if (parse_port(&PORT(mgr), strtrim(input)) < 0)
				hermes_error(INPUT_ERROR, TRUE, 2, "parsing ports", strerror(errno));
			add_node_list_head(&(mgr->worker_set.wrkrs), &worker);
			mgr->worker_set.wrkr_cnt++;
			free(worker);
		}
		else if (!memcmp("del", input, 3))
		{
			prompt("ips > ", input, 20);
			if (parse_ip(&ips, strtrim(input)) < 0)
				hermes_error(INPUT_ERROR, TRUE, 2, "parsing ips", strerror(errno));
			remove_node_list_head(&mgr->worker_set.wrkrs);
			mgr->worker_set.wrkr_cnt--;
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
