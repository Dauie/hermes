#include "../../incl/job.h"
#include "../../incl/parser.h"

void 			h_ack_portset(t_mgr *mgr, char *input)
{
	if (handle_port(&mgr->job.ack_ports, input) == FAILURE)
		return ; /* TODO hermes_error() */
	mgr->job.opts.bitops.do_ack_discov = TRUE;
}

void 			h_scan_portset(t_mgr *mgr, char *input)
{
	if (handle_port(&mgr->job.ports, input) == FAILURE)
		return ; /* TODO hermes_error() */
	mgr->job.opts.bitops.scan_ports_spec = TRUE;
}

void 			h_syn_portset(t_mgr *mgr, char *input)
{
	if (handle_port(&mgr->job.syn_ports, input) == FAILURE)
		return ; /* TODO hermes_error() */
	mgr->job.opts.bitops.do_syn_discov = TRUE;
}

void 			h_udp_portset(t_mgr *mgr, char *input)
{
	if (handle_port(&mgr->job.udp_ports, input) == FAILURE)
		return ; /* TODO hermes_error() */
	mgr->job.opts.bitops.do_udp_discov = TRUE;
}

void			h_exclude_ports(t_mgr *mgr, char *input)
{
	if (handle_port(&mgr->exclude_ports, input) == FAILURE)
		return ; /* TODO hermes_error() */
	mgr->job.opts.bitops.exclude_ports_spec = TRUE;
}

void			h_dont_randomize_ports(t_mgr *mgr)
{
	mgr->job.opts.bitops.ports_no_random = TRUE;
}
