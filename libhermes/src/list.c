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


int				add_node(t_node **tree, t_node **node, int (*cmp)(void *, void *))
{
	t_node		*cur;
	t_node		*prnt;
	int			ret;

	prnt = NULL;
	if (!*tree)
	{
		*tree = *node;
		return (SUCCESS);
	}
	prnt = cur = *tree;
	while (cur)
	{
		prnt = cur;
		ret = cmp(cur->data, node);
		if (ret < 0)
			cur = cur->left;
		else if (ret > 0)
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
		prnt->right = (*node)->data;
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


//int			remove_node(t_node **tree, void *data, void *(*min)(t_node *),
//						int (*cmp)(void *, void *))
//{
//	t_node		*parent;
//	t_node		*child;
//	t_node		*node;
//	void		*save;
//
//	node = *tree;
//	parent = NULL;
//	while (node && cmp(node->data, data) != 0)
//	{
//		parent = node;
//		if (cmp(node->data, data) < 0)
//			node = node->left;
//		else if (cmp(node->data, data) > 0)
//			node = node->right;
//	}
//	if (!node)
//		return (FAILURE);
//	if (node == *tree)
//	{
//		del_node(tree);
//		return (SUCCESS);
//	}
//	if (!node->left && !node->right)
//	{
//		if (parent->left == node)
//			parent->left = NULL;
//		else
//			parent->right = NULL;
//		del_node(&node);
//	}
//	else if (node->left && node->right)
//	{
//		save = min(node->right);
//		remove_node(&node, save, min, cmp);
//		node->data = save;
//	}
//	else
//	{
//		child = node->left ? node->left : node->right;
//		if (node != *tree)
//		{
//			if (node == parent->left)
//				parent->left = child;
//			else
//				parent->right = child;
//		}
//		else
//			*tree = child;
//		del_node(&node);
//	}
//	return (SUCCESS);
//}


//void		delete_tree(t_node **tree)
//{
//	if (tree && *tree)
//	{
//
//	}
//}