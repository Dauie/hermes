#include "../incl/hermes.h"
#include "../incl/binnify.h"
#include "../incl/message.h"

void	distribute_obj(t_node **wrkr_tree, uint8_t type, binn *obj)
{
	t_wrkr *wrkr;

	if ((*wrkr_tree)->left)
		distribute_obj(&(*wrkr_tree)->left, type, obj);

	wrkr = (*wrkr_tree)->data;
	(void)wrkr;

	if ((*wrkr_tree)->right)
		distribute_obj(&(*wrkr_tree)->right, type, obj);
}

bool		send_work(t_mgr *mgr, t_wrkr *wrkr)
{
	binn		*targets;

	if (!wrkr || !mgr)
		return (false);
	mgr->job.targets->total -= partition_targetset(
			wrkr->job->targets,
			mgr->job.targets,
			wrkr->send_size);
	targets = binnify_targetset(wrkr->job->targets);
	if (hermes_send_binn(
		wrkr->sock,
		C_OBJ_TARGETS,
		targets) < 0)
		hermes_error(SUCCESS, "hermes_send_binn() failed");
	// TODO : increment wrkr->send_size by an amount (double?)
	return (true);
}
