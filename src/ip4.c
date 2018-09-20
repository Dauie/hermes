#include "../incl/job.h"

t_ip4			*new_ip4(void)
{
	t_ip4		*data;

	if (!(data = (t_ip4*)memalloc(sizeof(t_ip4))))
	{
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
		return (NULL);
	}
	return (data);
}

t_ip4range		*new_ip4range(void)
{
	t_ip4range	*data;

	if (!(data = (t_ip4range*)memalloc(sizeof(t_ip4range))))
	{
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
		return (NULL);
	}
	return (data);
}

int				ip4_cmp(void *ip_left, void *ip_right)
{
	t_ip4		*left;
	t_ip4		*right;

	left = ip_left;
	right = ip_right;
	if (left->addr < right->addr)
		return (-1);
	else if (left->addr > right->addr)
		return (1);
	else
		return (0);
}

void			ip4_del(t_node **node)
{
	t_ip4		*del;

	if (!node || !*node)
		return ;
	del = (*node)->data;
	if (del)
		free(del);
	free(*node);
}

void			*ip4_min(t_node *tree)
{
	t_ip4		*save;

	if (!tree)
		return (NULL);
	save = new_ip4();
	while (tree->left)
		tree = tree->left;
	memcpy(save, tree->data, sizeof(t_ip4));
	return (save);
}

int				ip4range_cmp(void *ipr_left, void *ipr_right)
{
	t_ip4range	*left;
	t_ip4range	*right;

	left = ipr_left;
	right = ipr_right;
	if (left->start == right->start)
	{
		if (left->end < right->end)
			return (-1);
		if (left->end > right->end)
			return (1);
		else
			return (0);
	}
	else if (left->start > right->start)
		return (1);
	else
		return (-1);
}

void			ip4range_del(t_node **node)
{
	t_ip4range	*del;

	if (!node || !*node)
		return ;
	del = (*node)->data;
	if (del)
		free(del);
	free(*node);
}

void			*ip4range_min(t_node *tree)
{
	t_ip4range	*save;

	if (!tree)
		return (NULL);
	save = new_ip4range();
	while (tree->left)
		tree = tree->left;
	memcpy(save, tree->data, sizeof(t_ip4range));
	return (save);
}