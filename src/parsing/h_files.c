#include "../../incl/parser.h"
#include "../../incl/hermes.h"


int			h_exclude_target_file(t_manager *mgr, char *input)
{
	if (!mgr->exclude_targets)
		mgr->exclude_targets = new_targetset();
	return (parse_ip_file(mgr->exclude_targets, input));
}


int			h_target_file(t_manager *mgr, char *input)
{
	return (parse_ip_file(mgr->job.targets, input));
}

int			h_worker_file(t_manager *mgr, char *input)
{
	return (parse_worker_file(mgr->workers, input));
}

