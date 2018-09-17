#include "../../incl/parser.h"

t_worker *new_worker(void) {
	t_worker *worker;

	if (!(worker = (t_worker*)memalloc(sizeof(t_worker))))
		return (NULL);
	return (worker);
}

void set_worker(t_worker *data, uint32_t ip, uint16_t port) {
	memcpy(&data->ip, &ip, sizeof(uint32_t));
	memcpy(&data->port, &port, sizeof(uint16_t));
}

void h_worker(t_node *worker_list, char *input) {
	uint32_t 	ip;
	uint16_t 	port;
	t_worker	*data;
	t_node		*node;
	char		**wlist; /* worker list */

	if (input) {
		wlist = strsplit(input, ',');
		while (wlist) {
			if (!(data = new_worker()))
				return;
			if (!(node = new_node()))
				return;
            if ((ip = get_ip(strsep(wlist, ":"))) < 0)
            	return;
			if ((port = get_port(wlist)) < 0)
				return;
			set_worker(&data, ip, port);
			set_node(node, data, sizeof(data));
			listadd_head(&worker_list, node);
            wlist++;
            if (data)
            	free(data);
        }
    }
}