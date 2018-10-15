#include "../../incl/hermes.h"
#include "../../incl/parser.h"

int			h_exclude_target_file(t_mgr *mgr, char *input)
{
	if (!mgr->exclude_targets)
		mgr->exclude_targets = new_targetset();
	if (parse_ip_file(mgr->exclude_targets, input) == FAILURE)
		hermes_error(EXIT_FAILURE, 1, "issue parsing exclude file");
	return (SUCCESS);
}


int			h_target_file(t_mgr *mgr, char *input)
{
	if (!mgr->job.targets)
		mgr->job.targets = new_targetset();
	if (parse_ip_file(mgr->job.targets, input) == FAILURE)
		hermes_error(EXIT_FAILURE, 1, "issue parsing target");
	return (SUCCESS);
}

int			h_worker_file(t_mgr *mgr, char *input)
{
	if (!mgr->workers)
		mgr->workers = new_workerset();
	if (parse_worker_file(mgr->workers, input) == FAILURE)
		hermes_error(EXIT_FAILURE, 1, "issue parsing worker");
	return (SUCCESS);
}

