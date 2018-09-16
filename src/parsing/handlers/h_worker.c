#include <job.h>
#include <hermese.h>
#include <string.h>
#include "../../incl/job.h"

t_worker *construct_worker(t_ip4 ip, t_port port) {
	t_worker *worker;

	worker = (t_worker*)memalloc(sizeof(t_worker));
	if (ip != NULL && port) {
		memmove(worker->ip, ip, sizeof(t_ip4));
		memmove(worker->port, por, sizeof(t_port));
		return (worker);
	}
	return (NULL);
}

void h_worker(t_job *job, char *input) {

    t_ip4		*ip_n;
	t_worker	*data;
	char		**wlist;
	t_port		*port_n;
	t_node		*worker;

	if (job && input) {
		wlist = strsplit(input, ',');
		while (wlist) {
            if (!(ip = strsep(wlist, ":")))
                return ;
                 //FAILURE
			if (!(data = construct_worker(
						construct_port(*wlist),
						construct_ip(ip)
					)))
				//FAILURE
				return;
			if (!(worker = construct_node(data, sizeof(data))))
				return (NULL);
			listadd_head(&job->worker_list, worker);
            wlist++;
        }
        return (SUCCESS);
    }
    return (FAILURE);
}