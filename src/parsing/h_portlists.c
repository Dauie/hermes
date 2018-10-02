#include "../../incl/job.h"
#include "../../incl/parser.h"

void 			h_ack_portlist(t_job *job, char *input)
{
	if (handle_port(&job->ack_ports, input) == FAILURE)
		return ; /* TODO hermes_error() */
	job->options.bitops.do_ack_discov = TRUE;
}

void 			h_scan_portlist(t_job *job, char *input)
{
	if (handle_port(&job->ports, input) == FAILURE)
		return ; /* TODO hermes_error() */
	job->options.bitops.scan_ports_spec = TRUE;
}

void 			h_syn_portlist(t_job *job, char *input)
{
	if (handle_port(&job->syn_ports, input) == FAILURE)
		return ; /* TODO hermes_error() */
	job->options.bitops.do_syn_discov = TRUE;
}

void 			h_udp_portlist(t_job *job, char *input)
{
	if (handle_port(&job->udp_ports, input) == FAILURE)
		return ; /* TODO hermes_error() */
	job->options.bitops.do_udp_discov = TRUE;
}

void			h_exclude_ports(t_job *job, char *input)
{
	if (handle_port(&job->exclude_ports, input) == FAILURE)
		return ; /* TODO hermes_error() */
	job->options.bitops.exclude_ports_spec = TRUE;
}

void			h_dont_randomize_ports(t_job *job)
{
	job->options.bitops.ports_no_random = TRUE;
}
