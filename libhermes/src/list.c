# include "sys/errno.h"
# include "../incl/libhermes.h"

t_node			*new_node(void)
{
	t_node		*node;

	if (!(node = (t_node*)memalloc(sizeof(t_node))))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	return (node);
}

void			del_node(t_node **node)
{
	if (!node || !*node)
		return ;
	if ((*node)->data)
	{
		free((*node)->data);
		(*node)->data = NULL;
	}
	free(*node);
	*node = NULL;
}

t_node			*bst_search(t_node **tree, void *data, int (*cmp)(void *, void *))
{
	t_node		*cur;

	cur = *tree;
	while (cur)
	{
		if (cmp(cur->data, data) < 0)
			cur = cur->left;
		else if (cmp(cur->data, data) > 0)
			cur = cur->right;
		else
			return (cur);
	}
	return (NULL);
}


int				add_node(t_node **tree, t_node **node,
							int (*cmp)(void *, void *))
{
	t_node		*cur;
	t_node		*prnt;

	if (!node)
		return (FAILURE);
	if (!*tree)
	{
		*tree = *node;
		return (SUCCESS);
	}
	prnt = cur = *tree;
	while (cur)
	{
		prnt = cur;
		if (cmp(cur->data, (*node)->data) < 0)
			cur = cur->left;
		else if (cmp(cur->data, (*node)->data) > 0)
			cur = cur->right;
		/* Keep duplicate nodes from being added*/
		else
		{
			del_node(node);
			return (FAILURE);
		}
	}
	if (cmp(prnt->data, (*node)->data) < 0)
		prnt->left = *node;
	else
		prnt->right = *node;
	return (SUCCESS);
}

static t_node			*bst_rm_search(t_node **tree, t_node **parent,
								void *data, int (*cmp)(void *, void *))
{
	t_node		*cur;
	t_node		*prnt;

	cur = *tree;
	while (cur)
	{
		prnt = cur;
		if (cmp(cur->data, data) < 0)
			cur = cur->left;
		else if (cmp(cur->data, data) > 0)
			cur = cur->right;
		else
		{
			*parent = prnt;
			return (cur);
		}
	}
	*parent = NULL;
	return (NULL);
}


int			remove_node(t_node **tree, void *data, void *(*min)(t_node *),
						int (*cmp)(void *, void *))
{
	t_node		*curr;
	t_node		*prnt;
	t_node		*chld;
	void		*save;

	prnt = NULL;
	if (!(curr = bst_rm_search(tree, &prnt, data, cmp)))
		return (FAILURE);
	if (!curr->left && !curr->right)
	{
		if (curr != *tree)
		{
			if (prnt->left == curr)
				prnt->left = NULL;
			else
				prnt->right = NULL;
		}
		else
			*tree = NULL;
		del_node(&curr);
	}
	else if (curr->left && curr->right)
	{
		save = min(curr->right);
		remove_node(&curr, save, min, cmp);
		curr->data = save;
	}
	else
	{
		chld = curr->left ? curr->left : curr->right;
		if (curr != *tree)
		{
			if (curr == prnt->left)
				prnt->left = chld;
			else
				prnt->right = chld;
		}
		else
			*tree = chld;
		del_node(&curr);
	}
	return (SUCCESS);
}

void		delete_tree(t_node **tree)
{
	if (tree && *tree)
	{

	}
}