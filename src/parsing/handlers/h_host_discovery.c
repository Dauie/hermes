#include "../../../incl/job.h"
#include "../../../incl/bool.h"

void			h_icmp_host_discovery(t_job *job)
{
	job->options.discops.echo = TRUE;
}

void			h_skip_host_discovery(t_job *job)
{
	job->options.discops.skip_discovery = TRUE;
}
void			h_tstamp_host_discovery(t_job *job)
{
	job->options.discops.timestamp = TRUE;
}
void			h_nmask_host_discovery(t_job *job)
{
	job->options.discops.netmask = TRUE;
}

void			h_traceroute(t_job *job)
{
	job->options.discops.tracert = TRUE;
}
