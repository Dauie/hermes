#include "../incl/hermes.h"

int				send_job(t_worker *worker, t_job *job)
{
	binn			*job_binn;

	job_binn = binnify_job(job);
	if (hermes_send_msg(worker->sock, type_code(JOB_MSG, JOB_OFFER), "") < 0)
		return (FAILURE);
	if (hermes_send_binn(worker->sock, type_code(JOB_MSG, JOB_DATA), job_binn) < 0)
		return (FAILURE);
	return (SUCCESS);
}
