#include "../incl/hermes.h"

/*
**	func | send_work()
**	param1 | t_worker | worker to send serialized job to.
**	param2 | t_job | job to be serialized and sent to worker.
**	return | int | SUCCESS (0) or FAILURE (-1)
**
**	desc: first job is serialized into a binn object, then a job offer is sent
**	to worker. If job offer is accepted by worker, the serialized job is
**	sent, else error is thrown. Job should be saved and re-distributed;
*/

/*TODO send sections of job separately*/
int					send_work(t_worker *worker, t_job *job, t_targetset *portion)
{
	binn			*job_binn;
	t_msg_hdr		*hdr;
	uint32_t		binn_len;
	uint8_t			rplybuff[PKT_SIZE];

	job_binn = binnify_job(job);
	binn_len = htonl((uint32_t)binn_size(job_binn));
	if (hermes_sendmsgf(worker->sock, msg_tc(MT_JOB, MC_JOB_OFFER), "u32",
						binn_len) == FAILURE)
		return (FAILURE);
	if (hermes_recvmsg(worker->sock, rplybuff) < 0)
		return (hermes_error(FAILURE, FALSE, 2, "worker disconnected early",
				inet_ntoa(worker->sin.sin_addr)));
	hdr = (t_msg_hdr*)rplybuff;
	if (hdr->type == MT_JOB_REPLY)
	{
		if (hdr->code == C_ACCEPT)
		{
			if (hermes_send_binn(worker->sock, job_binn, binn_len) == FAILURE)
				return (FAILURE);
			if (hermes_recvmsg(worker->sock, rplybuff) == FAILURE)
				return (hermes_error(FAILURE, FALSE, 2,
						"worker disconnected early",
						inet_ntoa(worker->sin.sin_addr)));
			hdr = (t_msg_hdr*)rplybuff;
			if (hdr->type == MT_JOB_REPLY)
			{
				if (hdr->code == C_RECV_CNFRM)
					return (SUCCESS);
				else if (hdr->code == C_RECV_FAIL)
					return (FAILURE);
			}
			else
				return (FAILURE);
		}
		else if (hdr->code == C_PARAM_ERR)
		{
			hermes_error(FAILURE, FALSE, 2,
					"hermes message parameter issue reported by worker",
					inet_ntoa(worker->sin.sin_addr));
			return (FAILURE);
		}
		else if (hdr->code == MC_JOB_DENY_BUSY)
		{
			hermes_error(FAILURE, FALSE, 2,
						inet_ntoa(worker->sin.sin_addr),
						"already has a job");
			return (FAILURE);
		}
		else if (hdr->code == C_DENY_OOM)
		{
			hermes_error(FAILURE, FALSE, 2,
						 inet_ntoa(worker->sin.sin_addr),
						 "is out of memory");
			return (FAILURE);
		}
		else
			return (FAILURE);
	}
	else
		return (FAILURE);
	return (SUCCESS);
}
