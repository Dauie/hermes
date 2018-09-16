#include "../../../incl/parser.h"
#include "../../../incl/defined.h"

void			h_thread_amt(t_job *job, char *input)
{
	int			threads;

	if ((threads = atoi(input)) <= 0 || threads > MAX_THREADS)
		return ; /* TODO hermese_error() */
	job->options.thread_count = (uint8_t)threads;
}

void			h_max_hostgroup(t_job *job, char *input)
{
	long		hgsize;

	if ((hgsize = atoi(input)) < MIN_HOSTGROUP || hgsize > MAX_HOSTGROUP)
		return ; /* TODO hermese_error () */
	job->options.rateops.max_hostgroup = (uint32_t)hgsize;
}

void			h_max_packet_rate(t_job *job, char *input)
{
	long		rate;

	if ((rate = atoi(input)) <= 0)
		return ; /* TODO hermese_error () */
	job->options.rateops.max_packet_rate = rate;
}

void			h_max_retries(t_job *job, char *input)
{
	long		retries;

	if ((retries = atoi(input)) < 0)
		return ; /* TODO hermese_error () */
	job->options.rateops.max_retries = (uint32_t)retries;
}

void			h_min_hostgroup(t_job *job, char *input)
{
	long		hgsize;

	if ((hgsize = atoi(input)) < MIN_HOSTGROUP || hgsize > MAX_HOSTGROUP)
		return ; /* TODO hermese_error () */
	job->options.rateops.min_hostgroup = (uint32_t)hgsize;
}

void			h_min_packet_rate(t_job *job, char *input)
{
	long		rate;

	if ((rate = atoi(input)) <= 0)
		return ; /* TODO hermese_error () */
	job->options.rateops.min_packet_rate = rate;
}
