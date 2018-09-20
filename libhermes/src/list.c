# include "sys/errno.h"
# include "../incl/libhermes.h"

t_node			*new_node(void)
{
	t_node		*node;

	if (!(node = (t_node*)memalloc(sizeof(t_node))))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	return (node);
}

t_node			*bst_search(t_node **tree, void *data, int (*cmp)(void *, void *))
{
	t_node		*cur;

	cur = *tree;
	while (cur)
	{
		if (cmp(cur->data, data) == 0)
			return (cur);
		if (cmp(cur->data, data) < 0)
			cur = cur->left;
		else if (cmp(cur->data, data) > 0)
			cur = cur->right;
	}
	return (NULL);
}


void			bst_add(t_node **tree, t_node **node,
						int (*cmp)(void *, void *), void (*del)(t_node **))
{
	t_node 		*cur;
	t_node		*prnt;

	if (!tree || !node || !*node)
		return ;
	prnt = cur = *tree;
	while (cur)
	{
		prnt = cur;
		if (cmp(cur->data, (*node)->data) < 0)
			cur = cur->left;
		else if (cmp(cur->data, (*node)->data) > 0)
			cur = cur->right;
		/* Keep duplicate nodes from being added*/
		else if (cmp(cur->data, (*node)->data) == 0)
		{
			del(node);
			return ;
		}
	}
	if (!prnt)
		*tree = *node;
	else if (cmp(prnt->data, (*node)->data) < 0)
		prnt->left = *node;
	else if (cmp(prnt->data, (*node)->data) > 0)
		prnt->right = *node;
}

t_node			*bst_rm_search(t_node **tree, t_node **parent,
								void *data, int (*cmp)(void *, void *))
{
	t_node		*cur;
	t_node		*prnt;

	cur = *tree;
	while (cur)
	{
		prnt = cur;
		if (cmp(cur->data, data) == 0)
		{
			*parent = prnt;
			return (cur);
		}
		if (cmp(cur->data, data) < 0)
			cur = cur->left;
		else if (cmp(cur->data, data) > 0)
			cur = cur->right;
	}
	*parent = NULL;
	return (NULL);
}


void			bst_remove(t_node **tree, void *data, void *(*min)(t_node *),
					int (*cmp)(void *, void *), void (*del)(t_node **))
{
	t_node		*cur;
	t_node		*prnt;
	t_node		*chld;
	void		*save;

	cur = bst_rm_search(tree, &prnt, data, cmp);
	if (!cur)
		return ;
	if (!cur->left && !cur->right)
	{
		if (cur != *tree)
		{
			if (prnt->left == cur)
				prnt->left = NULL;
			else
				prnt->right = NULL;
		}
		else
			*tree = NULL;
		del(&cur);
	}
	else if (cur->left && cur->right)
	{
		save = min(cur->right);
		bst_remove(tree, save, min, cmp, del);
		cur->data = save;
	}
	else
	{
		chld = cur->left ? cur->left : cur->right;
		if (cur != *tree)
		{
			if (cur == prnt->left)
				prnt->left = chld;
			else
				prnt->right = chld;
		}
		else
			*tree = chld;
		del(&cur);
	}
}