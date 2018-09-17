#include "../../../incl/job.h"
#include "../../../libhermes/incl/bool.h"

void			h_icmp_host_discovery(t_job *job)
{
	job->options.bitops.do_echo_discov = TRUE;
}

void			h_skip_host_discovery(t_job *job)
{
	job->options.bitops.skip_hst_discov = TRUE;
}
void			h_tstamp_host_discovery(t_job *job)
{
	job->options.bitops.do_tstamp_discov = TRUE;
}
void			h_nmask_host_discovery(t_job *job)
{
	job->options.bitops.do_nmask_discov = TRUE;
}

void			h_traceroute(t_job *job)
{
	job->options.bitops.do_tracert = TRUE;
}
