#include "../incl/hermes.h"
#include "../incl/job.h"
#include "../incl/defined.h"

int			sanity_check(t_job *job)
{
	if (job->options.min_rtt_timeo == 0)
		job->options.min_rtt_timeo = DEF_MIN_RTT_TIMEOUT;
	if (job->options.max_rtt_timeo == 0)
		job->options.max_rtt_timeo = DEF_MAX_RTT_TIMEOUT;
	if (job->options.init_rtt_timeo == 0)
		job->options.init_rtt_timeo = DEF_INIT_RTT_TIMEOUT;
	if (job->options.max_retries == 0)
		job->options.max_retries = DEF_MAX_RETRIES;
	do_exclusions(&job->targets, &job->exclude_targets);
	return (SUCCESS);
}