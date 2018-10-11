#include "../incl/hermes.h"

/*
**	func | send_work()
**	param1 | t_wrkr | worker to send serialized job to.
**	param2 | t_job | job to be serialized and sent to worker.
**	return | int | SUCCESS (0) or FAILURE (-1)
**
**	desc: first job is serialized into a binn object, then a job offer is sent
**	to worker. If job offer is accepted by worker, the serialized job is
**	sent, else error is thrown. Job should be saved and re-distributed;
*/

/*TODO send sections of job separately*/

void	distribute_obj(t_node **wrkr_tree, uint8_t type, binn *obj)
{
	t_wrkr *wrkr;

	if ((*wrkr_tree)->left)
		distribute_obj(&(*wrkr_tree)->left, type, obj);

	wrkr = (*wrkr_tree)->data;



	if ((*wrkr_tree)->right)
		distribute_obj(&(*wrkr_tree)->right, type, obj);
}

int			send_work(t_node **wrkr_tree, t_job *job)
{
	binn	*opts;
	binn	*ports;
	binn	*ack_ports;
	binn	*syn_ports;
	binn	*udp_ports;

	opts = binnify_opts(&job->opts);
	ports = binnify_portset(job->ports);
	ack_ports = binnify_portset(job->ack_ports);
	syn_ports = binnify_portset(job->syn_ports);
	udp_ports = binnify_portset(job->udp_ports);

	return(SUCCESS);
}
