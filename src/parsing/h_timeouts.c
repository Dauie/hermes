#include "../../incl/parser.h"

int			h_host_timeout(t_mgr *mgr, char *input)
{
	if (!input)
		return (hermes_error(FAILURE, 1, "--host-timeout not specified"));
	if (parse_time(&mgr->job.opts.host_timeo, input) == FAILURE)
		return (hermes_error(FAILURE, 1, "bad host-timeout specified"));
	return (SUCCESS);
}

int			h_max_rtt_timeout(t_mgr *mgr, char *input)
{
	if (!input)
		return (hermes_error(FAILURE, 1, "--max-rtt-timeout not specified"));
	if (parse_time(&mgr->job.opts.max_rtt_timeo, input) == FAILURE)
		return (hermes_error(FAILURE, 1, "bad max-rtt-timeout specified"));
	return (SUCCESS);
}

int			h_min_rtt_timeout(t_mgr *mgr, char *input)
{
	if (!input)
		return (hermes_error(FAILURE, 1, "--min-rtt-timeout not specified"));
	if (parse_time(&mgr->job.opts.min_rtt_timeo, input) == FAILURE)
		return (hermes_error(FAILURE, 1, "bad min-rtt-timeout specified"));
	return (SUCCESS);
}

int			h_scan_delay(t_mgr *mgr, char *input)
{
	if (!input)
		return (hermes_error(FAILURE, 1, "--scan-delay not specified"));
	if (parse_time(&mgr->job.opts.scan_delay, input) == FAILURE)
		return (hermes_error(FAILURE, 1, "bad scan-delay specified"));
	return (SUCCESS);
}

int			h_max_scan_delay(t_mgr *mgr, char *input)
{
	if (!input)
		return (hermes_error(FAILURE, 1, "--max-scan-delay not specified"));
	if (parse_time(&mgr->job.opts.max_scan_delay, input) == FAILURE)
		return (hermes_error(FAILURE, 1, "bad max-scan-delay specified"));
	return (SUCCESS);
}
