#include "../../incl/hermes.h"
#include "../../incl/parser.h"

int			h_ack_portset(t_mgr *mgr, char *input)
{
	if (!input)
		return (hermes_error(EXIT_FAILURE, "no ack port_stats specified"));
	if (!mgr->env.ack_ports)
		mgr->env.ack_ports = new_portset();
	if (handle_port(mgr->env.ack_ports, input) == FAILURE)
		return (hermes_error(EXIT_FAILURE, "issue parsing ack port_stats")) ;
	mgr->env.opts.bitops.do_ack_discov = true;
	return (SUCCESS);
}

int			h_scan_portset(t_mgr *mgr, char *input)
{
	if (!input)
		return (hermes_error(EXIT_FAILURE, "no port_stats specified"));
	if (handle_port(&mgr->env.ports, input) == FAILURE)
		return (hermes_error(EXIT_FAILURE, "issue parsing port_stats"));
	return (SUCCESS);
}

int			h_syn_portset(t_mgr *mgr, char *input)
{
//	if (!input)
//	{
//		if (!mgr->env.syn_ports)
//			mgr->env.syn_ports = new_portset();
//		if (handle_port(mgr->env.syn_ports, "80") == FAILURE)
//			return (hermes_error(EXIT_FAILURE, "issue parsing syn port_stats"));
//	}
	if (handle_port(mgr->env.syn_ports, input) == FAILURE)
		return (hermes_error(EXIT_FAILURE, "issue parsing syn port_stats"));
	mgr->env.opts.bitops.do_syn_discov = true;
	return (SUCCESS);
}

int			h_udp_portset(t_mgr *mgr, char *input)
{
	if (!input)
		return (hermes_error(EXIT_FAILURE, "no udp port_stats specified"));
	if (!mgr->env.udp_ports)
		mgr->env.udp_ports = new_portset();
	if (handle_port(mgr->env.udp_ports, input) == FAILURE)
		return hermes_error(EXIT_FAILURE, "issue parsing udp port_stats");
	mgr->env.opts.bitops.do_udp_discov = true;
	return (SUCCESS);
}

int			h_exclude_ports(t_mgr *mgr, char *input)
{
	if (!input)
		return (hermes_error(EXIT_FAILURE, "no exclude port_stats specified"));
	if (!mgr->exclude_ports)
		mgr->exclude_ports = new_portset();
	if (handle_port(mgr->exclude_ports, input) == FAILURE)
		return (hermes_error(EXIT_FAILURE, "issue parsing exclude port_stats"));
	return (SUCCESS);
}

int			h_dont_randomize_ports(t_mgr *mgr)
{
	mgr->env.opts.bitops.no_rand_ports = true;
	return (SUCCESS);
}
