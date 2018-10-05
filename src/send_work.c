#include "../incl/hermes.h"

int				send_work(t_worker *worker, t_job *job)
{
	binn			*work;
	binn			*ports;
	binn			*ops;
	binn			*targets;

	work = binn_object();
	ops = make_ops_binn(&job->options);
	targets = make_targetlist_binn(&job->targets);
	ports = make_portlist_binn(&job->ports);

	binn_object_set_object(work, "targets", targets);
	binn_object_set_object(work, "ports", ports);
	binn_object_set_object(work, "ops", ops);
	printf("ops size: %d\n", binn_size(ops));
	printf("targets size: %d\n",binn_size(targets));
	printf("ports size: %d\n",binn_size(ports));

	return (SUCCESS);
}