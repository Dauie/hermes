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
	if (left->ip == right->ip)
		return (0);
	else if (left->ip > right->ip)
		return (1);
	else
		return (-1);
}

void			worker_del(t_node **node)
{
	t_worker	*del;

	if (!node || !*node)
		return ;
	del = (*node)->data;
	if (del)
		free(del);
	free(*node);
}

void			*worker_min(t_node *tree)
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