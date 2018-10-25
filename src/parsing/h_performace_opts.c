#include "../../incl/hermes.h"

int				h_thread_amt(t_mgr *mgr, char *input)
{
	int			threads;

	if (!input)
		return (hermes_error(EXIT_FAILURE, "thread amount not specified"));
	if ((threads = atoi(input)) <= 0 || threads > MAX_THREADS)
		return (hermes_error(EXIT_FAILURE, "bad thread amount %s", input));
	mgr->job.opts.thread_count = (uint8_t)threads;
	return (SUCCESS);
}

int				h_max_hostgroup(t_mgr *mgr, char *input)
{
	long		hgsize;

	if (!input)
		return (hermes_error(EXIT_FAILURE, "--max-hostgroup not specified"));
	if ((hgsize = atoi(input)) < MIN_HOSTGROUP || hgsize > MAX_HOSTGROUP)
		return (hermes_error(EXIT_FAILURE, "bad max-hostgroup value %s", input));
	mgr->job.opts.max_hostgroup = (uint32_t)hgsize;
	return (SUCCESS);
}

int				h_min_hostgroup(t_mgr *mgr, char *input)
{
	long		hgsize;

	if (!input)
		return (hermes_error(EXIT_FAILURE, "--min-hostgroup not specified"));
	if ((hgsize = atoi(input)) < MIN_HOSTGROUP || hgsize > MAX_HOSTGROUP)
		return (hermes_error(EXIT_FAILURE, "bad min-hostgroup value %s", input));
	mgr->job.opts.min_hostgroup = (uint32_t)hgsize;
	return (SUCCESS);
}

int				h_max_packet_rate(t_mgr *mgr, char *input)
{
	long		rate;

	if (!input)
		return (hermes_error(EXIT_FAILURE, "--max-rate not specified"));
	if ((rate = atoi(input)) <= 0)
		return (hermes_error(EXIT_FAILURE, "bad max-packet-rate value %s", input));
	mgr->job.opts.max_packet_rate = (uint32_t)rate;
	return (SUCCESS);
}

int				h_min_packet_rate(t_mgr *mgr, char *input)
{
	long		rate;
	if (!input)
		return (hermes_error(EXIT_FAILURE, "--min-rate not specified"));
	if ((rate = atoi(input)) <= 0)
		return (hermes_error(EXIT_FAILURE, "bad min-packet-rate value %s", input));
	mgr->job.opts.min_packet_rate = (uint32_t)rate;
	return (SUCCESS);
}

int				h_max_retries(t_mgr *mgr, char *input)
{
	long		retries;

	if (!input)
		return (hermes_error(EXIT_FAILURE, "--max-retries not specified"));
	if ((retries = atoi(input)) < 0)
		return (hermes_error(EXIT_FAILURE, "bad max-retries value %s", input));
	mgr->job.opts.max_retries = (uint32_t)retries;
	return (SUCCESS);
}
