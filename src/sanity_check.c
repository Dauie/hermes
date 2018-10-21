#include "../incl/parser.h"
#include "../incl/hermes.h"

int			sanity_check(t_mgr *mgr)
{
	char	**range;

	if (mgr->job.opts->min_rtt_timeo == 0)
		mgr->job.opts->min_rtt_timeo = DEF_MIN_RTT_TIMEOUT;
	if (mgr->job.opts->max_rtt_timeo == 0)
		mgr->job.opts->max_rtt_timeo = DEF_MAX_RTT_TIMEOUT;
	if (mgr->job.opts->init_rtt_timeo == 0)
		mgr->job.opts->init_rtt_timeo = DEF_INIT_RTT_TIMEOUT;
	if (mgr->job.opts->max_retries == 0)
		mgr->job.opts->max_retries = DEF_MAX_RETRIES;
	if (!mgr->job.targets || mgr->job.targets->total == 0)
		return (hermes_error(FAILURE, "no targets specified"));
	else
	{
		if (mgr->job.targets->ips)
			mgr->job.targets->ips = bst_to_clist(mgr->job.targets->ips);
		if (mgr->job.targets->iprngs)
			mgr->job.targets->iprngs = bst_to_clist(mgr->job.targets->iprngs);
	}
	if (!mgr->job.ports || mgr->job.ports->total == 0)
	{
		if (!mgr->job.ports)
			mgr->job.ports = new_portset();
		range = strsplit("1-1024", '-');
		add_range_portset(mgr->job.ports, range);
		tbldel(&range);
	}
	else
	{
		if (mgr->job.ports->ports)
			mgr->job.ports->ports = bst_to_clist(mgr->job.ports->ports);
		if (mgr->job.ports->prtrngs)
			mgr->job.ports->prtrngs = bst_to_clist(mgr->job.ports->prtrngs);
	}
	return (SUCCESS);
}
