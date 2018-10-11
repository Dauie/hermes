#include "../../incl/hermes.h"

t_workerset	*new_workerset()
{
	t_workerset *set;

	if (!(set = (t_workerset*)memalloc(sizeof(t_workerset))))
		hermes_error(FAILURE, 2, "malloc()", strerror(errno));
	return (set);
}

t_wrkr		*new_worker(void)
{
	t_wrkr	*worker;

	if (!(worker = (t_wrkr*)memalloc(sizeof(t_wrkr))))
		hermes_error(FAILURE, 2, "malloc()", strerror(errno));
	return (worker);
}

int			worker_cmp(void *wrkr_left, void *wrk_right)
{
	t_wrkr	*left;
	t_wrkr	*right;

	left = (t_wrkr*)wrkr_left;
	right = (t_wrkr*)wrk_right;
	return (ip4_cmp(&left->sin.sin_addr, &right->sin.sin_addr));
}

void		*worker_min(t_node *tree)
{
	t_wrkr	*save;

	if (!tree)
		return (NULL);
	save = new_worker();
	while (tree->left)
		tree = tree->left;
	memcpy(save, tree->data, sizeof(t_wrkr));
	return (save);
}

