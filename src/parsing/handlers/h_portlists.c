#include "../../../incl/job.h"
#include "../../../incl/parser.h"

void 			h_ack_portlist(t_job *job, char *input)
{
	job->options.discops.ack = TRUE;
	if (parse_port(&job->d_ack_portlist, input) == FAILURE)
		return ; /* TODO hermese_error() */
}

void 			h_scan_portlist(t_job *job, char *input)
{
	if (parse_port(&job->scan_portlist, input) == FAILURE)
		return ; /* TODO hermese_error() */
}

void 			h_syn_portlist(t_job *job, char *input)
{
	job->options.discops.syn = TRUE;
	if (parse_port(&job->d_syn_portlist, input) == FAILURE)
		return ; /* TODO hermese_error() */
}

void 			h_udp_portlist(t_job *job, char *input)
{
	job->options.discops.udp = TRUE;
	if (parse_port(&job->d_udp_portlist, input) == FAILURE)
		return ; /* TODO hermese_error() */
}

void			h_exclude_ports(t_job *job, char *input)
{
	job->options.portops.exclude_ports = TRUE;
	if (parse_port(&job->exclude_portlist, input) == FAILURE)
		return ; /* TODO hermese_error() */
}

