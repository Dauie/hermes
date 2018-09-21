#include "../../incl/parser.h"

int				parse_worker(t_workerlist *workerlist, char *input)
{
	char		*worker;
	char		*ip_str;
	char		*port_str;
	uint32_t	ip;
	uint16_t	port;
	t_worker	*data;
	t_node		*node;

	if (input == NULL)
		hermes_error(INPUT_ERROR, TRUE, 1, "no ip:port provided for worker");
	while ((worker = strsep(&input, ",")))
	{
		port_str = worker;
		if (!(ip_str = strsep(&port_str, ":")))
			return (hermes_error(INPUT_ERROR, FALSE, 2, "no port specified for worker", worker));
		if ((parse_ip(&ip, ip_str)) == FAILURE)
			return (hermes_error(INPUT_ERROR, FALSE, 2, "bad ip provided for worker", ip_str));
		if ((parse_port(&port, port_str)) == FAILURE)
			return (hermes_error(INPUT_ERROR, FALSE, 3, "bad port provided for worker", ip_str, port_str));
		data = new_worker();
		node = new_node();
		data->ip = ip;
		data->port = port;
		node->data = data;
		if (add_node(&workerlist->workers, &node, worker_cmp) == SUCCESS);
			workerlist->worker_count++;
	}
	return (SUCCESS);
}

void			h_worker(t_job *job, char *input)
{
	parse_worker(&job->worker_list, input);
}

