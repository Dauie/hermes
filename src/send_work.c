#include "../incl/hermes.h"

/*
**	func | send_job()
**	param1 | t_worker | worker to send serialized job to.
**	param2 | t_job | job to be serialized and sent to worker.
**	return | int | SUCCESS (0) or FAILURE (-1)
**
**	desc: first job is serialized into a binn object, then a job offer is sent
**	to worker. If job offer is accepted by worker, the serialized job is
**	sent, else error is thrown.
*/

int					send_job(t_worker *worker, t_job *job)
{
	binn			*job_binn;
	t_msg_hdr		*hdr;
	uint32_t		binn_len;
	uint8_t			rplybuff[PKT_SIZE];

	job_binn = binnify_job(job);
	binn_len = htonl((uint32_t)binn_size(job_binn));
	if (hermes_send_msg(worker->sock, type_code(MT_JOB, MC_JOB_OFFER), "u32", binn_len) == FAILURE)
		return (FAILURE);
	if (hermes_recv_msg(worker->sock, rplybuff) < 0)
		return (hermes_error(FAILURE, FALSE, 2, "worker disconnected early",
				inet_ntoa(worker->sin.sin_addr)));
	hdr = (t_msg_hdr*)rplybuff;
	if (hdr->type == MT_JOB_REPLY && hdr->code == MC_JOB_ACCEPT)
	{
		if (hermes_send_binn(worker->sock, job_binn, binn_len))
			return (FAILURE);
	}
	return (SUCCESS);
}
