#include "../../incl/hermes.h"
#include "../../incl/parser.h"


int				parse_worker(t_workerset *set, char *input)
{
	char		*worker;
	char		*ip_str;
	char		*port_str;
	in_addr_t	ip;
	uint16_t	port;
	t_wrkr		*data;

	if (input == NULL)
		hermes_error(EXIT_FAILURE, 1, "no ips:ports provided for worker");
	while ((worker = strsep(&input, ",")))
	{
		port_str = worker;
		if (!(ip_str = strsep(&port_str, ":")))
			return (hermes_error(EXIT_FAILURE, 2, "no ports specified for worker:", worker));
		if ((parse_ip(&ip, ip_str)) == FAILURE)
			return (hermes_error(EXIT_FAILURE, 2, "bad ips provided for worker:", ip_str));
		if ((parse_port(&port, port_str)) == FAILURE)
			return (hermes_error(EXIT_FAILURE, 3, "bad ports provided for worker:", ip_str, port_str));
		data = new_worker();
		data->sin.sin_addr.s_addr = ip;
		data->sin.sin_port = htons(port);
		data->sin.sin_family = AF_INET;
		if (add_node_bst(&set->wrkrs, (void **) &data, worker_cmp) == true)
			set->cnt++;
	}
	return (SUCCESS);
}

int				h_worker(t_mgr *mgr, char *input)
{
	if (!input)
		return (hermes_error(EXIT_FAILURE, 1, "no wrkrs specified"));
	if (!mgr->workers)
		mgr->workers = new_workerset();
	return (parse_worker(mgr->workers, input));
}

int				h_daemon(t_mgr *mgr, char *input)
{
	uint16_t	port;

	(void)mgr;
	if (!input)
		return (hermes_error(EXIT_FAILURE, 1, "no ports specified for daemon"));
	if (parse_port(&port, input) == FAILURE)
		return (hermes_error(EXIT_FAILURE, 2, "bad ports given for daemon:", input));
	return (worker_daemon(port));
}
