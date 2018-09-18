# include "../../incl/job.h"
# include "../../incl/parser.h"
# include "../../libhermes/incl/bool.h"


void			h_exclude_target_file(t_job *job, char *input)
{
	parse_ip_file(&job->exclude_targets, input);
}


void			h_target_file(t_job *job, char *input)
{
	parse_ip_file(&job->targets, input);
}
