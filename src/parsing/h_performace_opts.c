#include "../../incl/parser.h"
#include "../../incl/defined.h"

int				h_thread_amt(t_mgr *mgr, char *input)
{
	int			threads;

	if ((threads = atoi(input)) <= 0 || threads > MAX_THREADS)
		return (hermes_error(FAILURE, 1, "bad thread amount"));
	mgr->job.opts.thread_count = (uint8_t)threads;
	return (SUCCESS);
}

int				h_max_hostgroup(t_mgr *mgr, char *input)
{
	long		hgsize;

	if (!input)
		return (hermes_error(FAILURE, true, 1, "--max-hostgroup not specified"));
	if ((hgsize = atoi(input)) < MIN_HOSTGROUP || hgsize > MAX_HOSTGROUP)
		return (hermes_error(FAILURE, 2, "bad max-hostgroup value", input));
	mgr->job.opts.max_hostgroup = (uint32_t)hgsize;
	return (SUCCESS);
}

int				h_min_hostgroup(t_mgr *mgr, char *input)
{
	long		hgsize;

	if (!input)
		return (hermes_error(FAILURE, 1, "--min-hostgroup not specified"));
	if ((hgsize = atoi(input)) < MIN_HOSTGROUP || hgsize > MAX_HOSTGROUP)
		return (hermes_error(FAILURE, 2, "bad min-hostgroup value", input));
	mgr->job.opts.min_hostgroup = (uint32_t)hgsize;
	return (SUCCESS);
}

int				h_max_packet_rate(t_mgr *mgr, char *input)
{
	long		rate;

	if (!input)
		return (hermes_error(FAILURE, 1, "--max-rate not specified"));
	if ((rate = atoi(input)) <= 0)
		return (hermes_error(FAILURE, 2, "bad max-packet-rate value", input));
	mgr->job.opts.max_packet_rate = (uint32_t)rate;
	return (SUCCESS);
}

int				h_min_packet_rate(t_mgr *mgr, char *input)
{
	long		rate;
	if (!input)
		return (hermes_error(FAILURE, 1, "--min-rate not specified"));
	if ((rate = atoi(input)) <= 0)
		return (hermes_error(FAILURE, 2, "bad min-packet-rate value", input));
	mgr->job.opts.min_packet_rate = (uint32_t)rate;
	return (SUCCESS);
}


int				h_max_retries(t_mgr *mgr, char *input)
{
	long		retries;

	if (!input)
		return (hermes_error(FAILURE, 1, "--max-retries not specified"));
	if ((retries = atoi(input)) < 0)
		return (hermes_error(FAILURE, 2, "bad max-retries value", input));
	mgr->job.opts.max_retries = (uint32_t)retries;
	return (SUCCESS);
}
