#include "../../incl/hermes.h"

int			h_ack_portset(t_manager *mgr, char *input)
{
	if (!mgr->job.ack_ports)
		mgr->job.ack_ports = new_portset();
	if (handle_port(mgr->job.ack_ports, input) == FAILURE)
		return (hermes_error(FAILURE, 1, "issue parsing ack ports")) ;
	mgr->job.opts.bitops.do_ack_discov = true;
	return (SUCCESS);
}

int			h_scan_portset(t_manager *mgr, char *input)
{
	if (!mgr->job.ports)
		mgr->job.ports = new_portset();
	if (handle_port(mgr->job.ports, input) == FAILURE)
		return (hermes_error(FAILURE, 1, "issue parsing ports"));
	return (SUCCESS);
}

int			h_syn_portset(t_manager *mgr, char *input)
{
	if (!mgr->job.syn_ports)
		mgr->job.syn_ports = new_portset();
	if (handle_port(mgr->job.syn_ports, input) == FAILURE)
		return (hermes_error(FAILURE, 1, "issue parsing syn ports"));
	mgr->job.opts.bitops.do_syn_discov = true;
	return (SUCCESS);
}

int			h_udp_portset(t_manager *mgr, char *input)
{
	if (!mgr->job.udp_ports)
		mgr->job.udp_ports = new_portset();
	if (handle_port(mgr->job.udp_ports, input) == FAILURE)
		return hermes_error(FAILURE, 1, "issue parsing udp ports");
	mgr->job.opts.bitops.do_udp_discov = true;
	return (SUCCESS);
}

int			h_exclude_ports(t_manager *mgr, char *input)
{
	if (!mgr->exclude_ports)
		mgr->exclude_ports = new_portset();
	if (handle_port(mgr->exclude_ports, input) == FAILURE)
		return (hermes_error(FAILURE, 1, "issue parsing exclude ports"));
	return (SUCCESS);
}

int			h_dont_randomize_ports(t_manager *mgr)
{
	mgr->job.opts.bitops.no_rand_ports = true;
	return (SUCCESS);
}
