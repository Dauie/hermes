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

void		del_tree(t_node **tree)
{
	if (!tree)
		return;
	if (!*tree)
		return;
	del_tree(&(*tree)->left);
	del_tree(&(*tree)->right);
	del_node(tree, true);
}

//t_node		**tree_to_array(t_node **tree)
//{
//	t_node **w_list;
//
//	w_list = (t_node*)
//	serialize(tree, &w_list, append);
//	del_tree(tree);
//	return (w_list);
//}

void		serialize(t_node **list, t_node **tree)
{
	if (!tree || !*tree)
		return;
	//enqueue(list, (*tree)->data);
	serialize(list, &(*tree)->left);
	enqueue(list, (*tree)->data);
	serialize(list, &(*tree)->right);
}

t_node		*tree_to_list(t_node **tree)
{
	t_node *w_list;

	serialize(&w_list, tree);
	del_tree(tree);
	return (w_list);
}

void		del_tree_nodes(t_node **tree)
{
	if (!tree)
		return;
	if (!*tree)
		return;
	del_tree_nodes(&(*tree)->left);
	del_tree_nodes(&(*tree)->right);
	(*tree)->data = NULL;
	free((*tree)->data);
	(*tree) = NULL;
	free(*tree);
}

t_node		*tree_search(t_node **tree, void *data, int (*cmp)(void *, void *))
{
	if (!*tree)
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

void		remove_search_key(t_node **curr, t_node **parent, void *key,
					int (*cmp)(void *, void *))
{
	while (*curr != NULL && cmp((*curr)->data, key) != 0)
	{
		*parent = *curr;
		if (cmp(key, (*curr)->data) < 0)
			*curr = (*curr)->left;
		else
			*curr = (*curr)->right;
	}
}

bool		remove_node_bst(t_node **tree, void *key, int (*cmp)(void *, void *), void *(*min)(t_node *))
{
	t_node	*parent = NULL;
	t_node	*curr = *tree;
	t_node	*child;
	void	*successor;

	remove_search_key(&curr, &parent, key, cmp);
	if (curr == NULL)
		return (false);
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
		remove_node_bst(tree, successor, cmp, min);
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
	return (true);
}