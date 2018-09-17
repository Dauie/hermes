#include "../../../incl/parser.h"

void			h_host_timeout(t_job *job, char *input)
{
	if (parse_time(&job->options.host_timeo, input) == FAILURE)
		return ; /*TODO hermes_error() */
}

void			h_max_rtt_timeout(t_job *job, char *input)
{
	if (parse_time(&job->options.max_rtt_timeo, input) == FAILURE)
		return ; /*TODO hermes_error() */
}

void			h_min_rtt_timeout(t_job *job, char *input)
{
	if (parse_time(&job->options.min_rtt_timeo, input) == FAILURE)
		return ; /*TODO hermes_error() */
}

void			h_scan_delay(t_job *job, char *input)
{
	if (parse_time(&job->options.scan_delay, input) == FAILURE)
		return ; /*TODO hermes_error() */
}

void			h_max_scan_delay(t_job *job, char *input)
{
	if (parse_time(&job->options.max_scan_delay, input) == FAILURE)
		return ; /*TODO hermes_error() */
}
