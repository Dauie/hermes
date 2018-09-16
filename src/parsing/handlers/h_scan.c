#include "../../../incl/job.h"
#include "../../../incl/bool.h"

void			h_scan_list(t_job *job)
{
	job->options.discops.list_scan = TRUE;
}

void			h_ping_scan(t_job *job)
{
	job->options.discops.ping_scan = TRUE;
}

void			h_syn_scan(t_job *job)
{
	job->options.scanops.syn = TRUE;
}

void			h_ack_scan(t_job *job)
{
	job->options.scanops.ack = TRUE;
}

void			h_null_scan(t_job *job)
{
	job->options.scanops.null = TRUE;
}

void			h_fin_scan(t_job *job)
{
	job->options.scanops.fin = TRUE;
}

void			h_xmas_scan(t_job *job)
{
	job->options.scanops.xmas = TRUE;
}

void			h_udp_scan(t_job *job)
{
	job->options.scanops.udp = TRUE;
}
