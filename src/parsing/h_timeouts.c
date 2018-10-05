#include "../../incl/parser.h"

void			h_host_timeout(t_mgr *mgr, char *input)
{
	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--host-timeout not specified");
	if (parse_time(&mgr->job.opts.host_timeo, input) == FAILURE)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad host-timeout specified");
}

void			h_max_rtt_timeout(t_mgr *mgr, char *input)
{
	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--max-rtt-timeout not specified");
	if (parse_time(&mgr->job.opts.max_rtt_timeo, input) == FAILURE)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad max-rtt-timeout specified");
}

void			h_min_rtt_timeout(t_mgr *mgr, char *input)
{
	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--min-rtt-timeout not specified");
	if (parse_time(&mgr->job.opts.min_rtt_timeo, input) == FAILURE)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad min-rtt-timeout specified");
}

void			h_scan_delay(t_mgr *mgr, char *input)
{
	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--scan-delay not specified");
	if (parse_time(&mgr->job.opts.scan_delay, input) == FAILURE)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad scan-delay specified");
}

void			h_max_scan_delay(t_mgr *mgr, char *input)
{
	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--max-scan-delay not specified");
	if (parse_time(&mgr->job.opts.max_scan_delay, input) == FAILURE)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad max-scan-delay specified");
}
