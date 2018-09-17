#include "../../incl/parser.h"
#include "../../incl/defined.h"

void			h_thread_amt(t_job *job, char *input)
{
	int			threads;

	if ((threads = atoi(input)) <= 0 || threads > MAX_THREADS)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad thread amount");
	job->options.thread_count = (uint16_t)threads;
}

void			h_max_hostgroup(t_job *job, char *input)
{
	long		hgsize;

	if ((hgsize = atoi(input)) < MIN_HOSTGROUP || hgsize > MAX_HOSTGROUP)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad max-hostgroup value");
	job->options.max_hostgroup = (uint32_t)hgsize;
}

void			h_max_packet_rate(t_job *job, char *input)
{
	long		rate;

	if ((rate = atoi(input)) <= 0)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad max-packet-rate value");
	job->options.max_packet_rate = (uint32_t)rate;
}

void			h_max_retries(t_job *job, char *input)
{
	long		retries;

	if ((retries = atoi(input)) < 0)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad max retries value");
	job->options.max_retries = (uint32_t)retries;
}

void			h_min_hostgroup(t_job *job, char *input)
{
	long		hgsize;

	if ((hgsize = atoi(input)) < MIN_HOSTGROUP || hgsize > MAX_HOSTGROUP)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad min-hostgroup value");
	job->options.min_hostgroup = (uint32_t)hgsize;
}

void			h_min_packet_rate(t_job *job, char *input)
{
	long		rate;

	if ((rate = atoi(input)) <= 0)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad min-packet-rate value");
	job->options.min_packet_rate = (uint32_t)rate;
}
