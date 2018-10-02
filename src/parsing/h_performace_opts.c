#include "../../incl/parser.h"
#include "../../incl/defined.h"

void			h_thread_amt(t_mgr *mgr, char *input)
{
	int			threads;

	if ((threads = atoi(input)) <= 0 || threads > MAX_THREADS)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad thread amount");
	mgr->job.options.thread_count = (uint8_t)threads;
}

void			h_max_hostgroup(t_mgr *mgr, char *input)
{
	long		hgsize;

	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--max-hostgroup not specified");
	if ((hgsize = atoi(input)) < MIN_HOSTGROUP || hgsize > MAX_HOSTGROUP)
		hermes_error(INPUT_ERROR, TRUE, 2, "bad max-hostgroup value", input);
	mgr->job.options.max_hostgroup = (uint32_t)hgsize;
}

void			h_min_hostgroup(t_mgr *mgr, char *input)
{
	long		hgsize;

	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--min-hostgroup not specified");
	if ((hgsize = atoi(input)) < MIN_HOSTGROUP || hgsize > MAX_HOSTGROUP)
		hermes_error(INPUT_ERROR, TRUE, 2, "bad min-hostgroup value", input);
	mgr->job.options.min_hostgroup = (uint32_t)hgsize;
}

void			h_max_packet_rate(t_mgr *mgr, char *input)
{
	long		rate;

	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--max-rate not specified");
	if ((rate = atoi(input)) <= 0)
		hermes_error(INPUT_ERROR, TRUE, 2, "bad max-packet-rate value", input);
	mgr->job.options.max_packet_rate = (uint32_t)rate;
}

void			h_min_packet_rate(t_mgr *mgr, char *input)
{
	long		rate;
	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--min-rate not specified");
	if ((rate = atoi(input)) <= 0)
		hermes_error(INPUT_ERROR, TRUE, 2, "bad min-packet-rate value", input);
	mgr->job.options.min_packet_rate = (uint32_t)rate;
}


void			h_max_retries(t_mgr *mgr, char *input)
{
	long		retries;

	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--max-retries not specified");
	if ((retries = atoi(input)) < 0)
		hermes_error(INPUT_ERROR, TRUE, 2, "bad max-retries value", input);
	mgr->job.options.max_retries = (uint32_t)retries;
}
