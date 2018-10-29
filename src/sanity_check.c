#include "../incl/parser.h"
#include "../incl/hermes.h"

int			sanity_check(t_mgr *mgr)
{
	if (mgr->env.opts.min_rtt_timeo == 0)
		mgr->env.opts.min_rtt_timeo = DEF_MIN_RTT_TIMEOUT;
	if (mgr->env.opts.max_rtt_timeo == 0)
		mgr->env.opts.max_rtt_timeo = DEF_MAX_RTT_TIMEOUT;
	if (mgr->env.opts.init_rtt_timeo == 0)
		mgr->env.opts.init_rtt_timeo = DEF_INIT_RTT_TIMEOUT;
	if (mgr->env.opts.max_retries == 0)
		mgr->env.opts.max_retries = DEF_MAX_RETRIES;
	if (mgr->env.opts.thread_count <= 1)
		mgr->env.opts.thread_count = 3;
	if (mgr->targets.total == 0)
		return (hermes_error(FAILURE, "no targets specified"));
	if (mgr->env.ports.total == 0)
	{
		add_range_portset(&mgr->env.ports, (char *[]){"1","1024"});
	}
	return (SUCCESS);
}
