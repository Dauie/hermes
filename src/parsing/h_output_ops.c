# include "../../incl/job.h"
# include "../../incl/parser.h"

void			h_append_output(t_job *job)
{
	job->options.bitops.apnd_file = TRUE;
}

void			h_norm_output(t_job *job)
{
	job->options.bitops.output_norm = TRUE;
}

void			h_show_only_open_ports(t_job *job)
{
	job->options.bitops.show_only_open_ports = TRUE;
}
