# include "../../incl/job.h"
# include "../../incl/parser.h"
# include "../../libhermes/incl/bool.h"


void			h_exclude_target_file(t_mgr *mgr, char *input)
{
	parse_ip_file(&mgr->exclude_targets, input);
}


void			h_target_file(t_mgr *mgr, char *input)
{
	parse_ip_file(&mgr->job.targets, input);
}

void			h_worker_file(t_mgr *mgr, char *input)
{
	parse_worker_file(&mgr->worker_set, input);
}

