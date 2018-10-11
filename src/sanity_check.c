#include "../incl/hermes.h"
#include "../incl/job.h"
#include "../incl/defined.h"


int			sanity_check(t_mgr *mgr)
{
	if (mgr->job.opts.min_rtt_timeo == 0)
		mgr->job.opts.min_rtt_timeo = DEF_MIN_RTT_TIMEOUT;
	if (mgr->job.opts.max_rtt_timeo == 0)
		mgr->job.opts.max_rtt_timeo = DEF_MAX_RTT_TIMEOUT;
	if (mgr->job.opts.init_rtt_timeo == 0)
		mgr->job.opts.init_rtt_timeo = DEF_INIT_RTT_TIMEOUT;
	if (mgr->job.opts.max_retries == 0)
		mgr->job.opts.max_retries = DEF_MAX_RETRIES;
	return (SUCCESS);
}