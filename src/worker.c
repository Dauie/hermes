#include <hermes.h>
#include "../incl/job.h"

t_worker		*new_worker(void)
{
	t_worker	*worker;

	if (!(worker = (t_worker*)memalloc(sizeof(t_worker))))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	return (worker);
}

int				worker_cmp(void *wrk_left, void *wrk_right)
{
	t_worker	*left;
	t_worker	*right;

	left = wrk_left;
	right = wrk_right;
	return (ip4_cmp(&left->sin.sin_addr, &right->sin.sin_addr));
}

void			*worker_min(t_bst *tree)
{
	t_worker	*save;

	if (!tree)
		return (NULL);
	save = new_worker();
	while (tree->left)
		tree = tree->left;
	memcpy(save, tree->data, sizeof(t_worker));
	return (save);
}

