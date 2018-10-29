#include "../../incl/hermes.h"

int			h_icmp_host_discovery(t_mgr *mgr)
{
	mgr->env.opts.bitops.do_echo_discov = true;
	return (SUCCESS);
}

int			h_skip_host_discovery(t_mgr *mgr)
{
	mgr->env.opts.bitops.skip_hst_discov = true;
	return (SUCCESS);
}
int			h_tstamp_host_discovery(t_mgr *mgr)
{
	mgr->env.opts.bitops.do_tstamp_discov = true;
	return (SUCCESS);
}
int			h_nmask_host_discovery(t_mgr *mgr)
{
	mgr->env.opts.bitops.do_nmask_discov = true;
	return (SUCCESS);
}

int			h_traceroute(t_mgr *mgr)
{
	mgr->env.opts.bitops.do_tracert = true;
	return (SUCCESS);
}
