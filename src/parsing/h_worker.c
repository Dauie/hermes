#include "../../incl/parser.h"
#include "../../incl/hermes.h"

int				parse_worker(t_workerlist *workerlist, char *input)
{
	char		*worker;
	char		*ip_str;
	char		*port_str;
	in_addr_t	ip;
	uint16_t	port;
	t_worker	*data;

	if (input == NULL)
		hermes_error(INPUT_ERROR, TRUE, 1, "no ips:ports provided for worker");
	while ((worker = strsep(&input, ",")))
	{
		port_str = worker;
		if (!(ip_str = strsep(&port_str, ":")))
			return (hermes_error(INPUT_ERROR, FALSE, 2, "no ports specified for worker:", worker));
		if ((parse_ip(&ip, ip_str)) == FAILURE)
			return (hermes_error(INPUT_ERROR, FALSE, 2, "bad ips provided for worker:", ip_str));
		if ((parse_port(&port, port_str)) == FAILURE)
			return (hermes_error(INPUT_ERROR, FALSE, 3, "bad ports provided for worker:", ip_str, port_str));
		data = new_worker();
		data->sin.sin_addr.s_addr = ip;
		data->sin.sin_port = htons(port);
		data->sin.sin_family = AF_INET;
		if (add_node(&workerlist->wrkrs, (void **)&data, worker_cmp) == SUCCESS)
			workerlist->wrkr_cnt++;
	}
	return (SUCCESS);
}

void			h_worker(t_mgr *mgr, char *input)
{
	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "no wrkrs specified");
	parse_worker(&mgr->worker_list, input);
}

void			h_daemon(t_mgr *mgr, char *input)
{
	uint16_t port;

	(void)mgr;
	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "no ports specified for daemon");
	if (parse_port(&port, input) == FAILURE)
		hermes_error(INPUT_ERROR, TRUE, 2, "bad ports given for daemon:", input);
	worker_daemon(port);
}
