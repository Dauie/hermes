#include "../../incl/hermes.h"
#include "../../incl/parser.h"

int			h_exclude_target_file(t_mgr *mgr, char *input)
{
	if (!mgr->exclude_targets)
		mgr->exclude_targets = new_targetset();
	if (parse_ip_file(mgr->exclude_targets, input) == FAILURE)
		hermes_error(EXIT_FAILURE, "issue parsing exclude targets from file");
	return (SUCCESS);
}


int			h_target_file(t_mgr *mgr, char *input)
{
	if (parse_ip_file(&mgr->targets, input) == FAILURE)
		hermes_error(EXIT_FAILURE, "issue parsing targets from file");
	return (SUCCESS);
}

int			h_worker_file(t_mgr *mgr, char *input)
{
	if (parse_worker_file(&mgr->workers, input) == FAILURE)
		hermes_error(EXIT_FAILURE, "issue parsing workers from file");
	return (SUCCESS);
}

