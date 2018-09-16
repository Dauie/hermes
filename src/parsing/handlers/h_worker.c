#include "../../incl/parser.h"

t_worker *construct_worker(uint32_t ip, uint16_t port) {
	t_worker *worker;

	worker = (t_worker*)memalloc(sizeof(t_worker));
	if (ip != NULL && port) {
		memmove(&worker->ip, &ip, sizeof(uint32_t));
		memmove(&worker->port, &port, sizeof(uint16_t));
		return (worker);
	}
	return (NULL);
}

void h_worker(t_job *job, char *input) {

	char 		*ip;
	uint32_t 	*ip_n;
	t_worker	*data;
	char		**wlist; /* worker list */
	t_node		*worker;


	if (job && input) {
		wlist = strsplit(input, ',');
		while (wlist) {
            if (!(ip = strsep(wlist, ":")))
                return ;
                 //FAILURE
            if (!inet_pton(AF_INET, ip, &ip_n))
            	return;
			if (!(data = construct_worker(
							ip_n,
							get_port(*wlist)
						)))
				//FAILURE
				return;
			if (!(worker = (t_node*)memalloc(sizeof(t_node))))
				return;
			worker->data = data;
			listadd_head(&job->worker_list, worker);
            wlist++;
            if (data)
            	free(data);
        }
        return;
    }
    return;
}