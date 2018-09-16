#include "../../../incl/job.h"
#include "../../../incl/bool.h"

void			h_scan_list(t_job *job)
{
	job->options.bitops.do_list_scan = TRUE;
}

void			h_ping_scan(t_job *job)
{
	job->options.bitops.do_ping_scan = TRUE;
}

void			h_syn_scan(t_job *job)
{
	job->options.bitops.do_syn_scan = TRUE;
}

void			h_ack_scan(t_job *job)
{
	job->options.bitops.do_ack_scan = TRUE;
}

void			h_null_scan(t_job *job)
{
	job->options.bitops.do_null_scan = TRUE;
}

void			h_fin_scan(t_job *job)
{
	job->options.bitops.do_fin_scan = TRUE;
}

void			h_xmas_scan(t_job *job)
{
	job->options.bitops.do_xmas_scan = TRUE;
}

void			h_udp_scan(t_job *job)
{
	job->options.bitops.do_udp_scan = TRUE;
}
