# include "sys/errno.h"
# include "../incl/libhermes.h"

t_node		*bst_search(t_node **tree, void *data, int (*cmp)(void *, void *))
{
	t_node	*cur;

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

void		del_tree(t_node **tree, bool deldata)
{
	if (!tree || !*tree)
		return;
	del_tree(&(*tree)->left, deldata);
	del_tree(&(*tree)->right, deldata);
	del_node(tree, deldata);
}

void		loop_tree_to_list(t_node **list, t_node **tree)
{
	if (!tree || !*tree)
		return ;
	if ((*tree)->left)
		loop_tree_to_list(&(*tree)->left, list);
	add_list_head(list, (*tree)->data);
	if ((*tree)->right)
		loop_tree_to_list(&(*tree)->right, list);
}

t_node		*tree_to_list(t_node **tree)
{
	t_node	*list;

	list = NULL;
	loop_tree_to_list(&list, tree);
	del_tree(tree, false);
	return (list);
}

t_node		*list_to_tree(t_node **list)
{
	t_node *tree;
	t_node *lst;

	tree = NULL;
	lst = list;
	while
}

t_node		*tree_search(t_node **tree, void *data, int (*cmp)(void *, void *))
{
	if (!(*tree))
		return (NULL);
	if ((*tree)->left)
		return (tree_search(&(*tree)->left, data, cmp));
	if (cmp(data, (*tree)->data) == 0)
		return (*tree);
	if ((*tree)->right)
		return (tree_search(&(*tree)->left, data, cmp));
	return (NULL);
}

bool		add_node_bst(t_node **root, void **data, int (*cmp)(void *, void *))
{
	int		ret;
	t_node	*node;
	t_node	*curr = *root;
	t_node	*parent = NULL;

	node = new_node(data);
	if (*root == NULL)
	{
		*root = node;
		return (true);
	}
	while (curr != NULL)
	{
		parent = curr;
		ret = cmp(*data, curr->data);
		if (ret < 0)
			curr = curr->left;
		else if (ret > 0)
			curr = curr->right;
		else
		{
			del_node(&node, NULL);
			return (false);
		}
	}
	if (cmp(data, parent->data) < 0)
		parent->left = node;
	else
		parent->right = node;
	return (true);
}

static void		remove_node_bst_search_key(t_node **cur, t_node **prnt, void *key,
							int (*cmp)(void *, void *))
{
	while (*cur != NULL && cmp((*cur)->data, key) != 0)
	{
		*prnt = *cur;
		if (cmp(key, (*cur)->data) < 0)
			*cur = (*cur)->left;
		else
			*cur = (*cur)->right;
	}
}

bool		rm_node_bst(t_node **tree, void *key,
						int (*cmp)(void *, void *), void *(*min)(t_node *))
{
	t_node	*parent = NULL;
	t_node	*curr = *tree;
	t_node	*child;
	void	*successor;

	remove_node_bst_search_key(&curr, &parent, key, cmp);
	if (curr == NULL)
		return (FAILURE);
	if (curr->left == NULL && curr->right == NULL)
	{
		if (cmp(curr->data, (*tree)->data) != 0)
		{
			if (parent->left == curr)
				parent->left = NULL;
			else
				parent->right = NULL;
		}
		else
			*tree = NULL;
		del_node(&curr, NULL);
	}
	else if (curr->left && curr->right)
	{
		successor  = min(curr->right);
		rm_node_bst(tree, successor, cmp, min);
		curr->data = successor;
	}
	else
	{
		child = (curr->left)? curr->left: curr->right;
		if (curr != *tree)
		{
			if (curr == parent->left)
				parent->left = child;
			else
				parent->right = child;
		}
		else
			*tree = child;
		del_node(&curr, NULL);
	}
	return (SUCCESS);
}