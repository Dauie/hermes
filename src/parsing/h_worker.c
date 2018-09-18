#include "../../incl/parser.h"

t_worker		*new_worker(void)
{
	t_worker	*worker;

	if (!(worker = (t_worker*)memalloc(sizeof(t_worker))))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	return (worker);
}

void			h_worker(t_job *job, char *input)
{
	uint32_t	ip;
	uint16_t	port;
	t_worker	*data;
	t_node		*node;
	char		*worker;
	char		*ip_str;
	char		*port_str;

	if (input == NULL)
		hermes_error(INPUT_ERROR, TRUE, 1, "no ip:port provided for worker");
	while ((worker = strsep(&input, ",")))
	{
		port_str = worker;
		if (!(ip_str = strsep(&port_str, ":")))
			hermes_error(INPUT_ERROR, TRUE, 2, "no port specified for worker", worker);
		if ((parse_ip(&ip, ip_str)) == FAILURE)
			hermes_error(INPUT_ERROR, TRUE, 2, "bad ip provided for worker", ip_str);
		if ((parse_port(&port, port_str)) == FAILURE)
			hermes_error(INPUT_ERROR, TRUE, 2, "bad port provided for worker", port_str);
		data = new_worker();
		node = new_node();
		data->ip = ip;
		data->port = port;
		node->data = data;
		listadd_head(&job->worker_list, node);
	}
}