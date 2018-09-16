#include <job.h>
#include <hermese.h>
#include <string.h>

void h_worker(t_job *job, char *input) {

    t_ip4		*ip_n;
    t_port		*port_n;
	t_worker	*data;
	char		**wlist;
	t_node		*worker;

	if (job && input) {
		wlist = ft_strsplit(input, ',');
		while (wlist) {
            if ((ip = strsep(*wlist, ':')) == NULL)
                return ;
                 //FAILURE
			ip = construct_node(t_ip4, construct_ip(ip));
			port = construct_node(t_port, construct_port(*wlist));
			worker = construct_node(t_worker, memmove());

			listadd_head(&job->worker_list, node);
            wlist++;
        }
    }
    // FAILURE
}