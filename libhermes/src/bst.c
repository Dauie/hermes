# include "sys/errno.h"
# include "../incl/libhermes.h"

void		tree_to_deq_help(t_node **tree, t_deque **deque)
{
	if (!tree || !*tree)
		return;
	tree_to_deq_help(&(*tree)->left, deque);
	enqueue(deque, (*tree)->data);
	tree_to_deq_help(&(*tree)->right, deque);
	del_node(tree, true);
}

t_deque		*tree_to_deque(t_tree **tree)
{
	t_deque *deque;

	if (!tree)
		return (NULL);
	deque = new_deque();
	tree_to_deq_help(&(*tree)->root, &deque);
	free(tree);
	tree = NULL;
	return (deque);
}

t_node		*bst_search(t_node **node, void *data, int (*cmp)(void *, void *))
{
	t_node	*cur;

	cur = *node;
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

t_node		*tree_search(t_node **root, void *data, int (*cmp)(void *, void *))
{
	if (!*root)
		return (NULL);
	if ((*root)->left)
		return (tree_search(&(*root)->left, data, cmp));
	if (cmp(data, (*root)->data) == 0)
		return (*root);
	if ((*root)->right)
		return (tree_search(&(*root), data, cmp));
	return (NULL);
}

bool		add_node_bst(t_node **root, void **data, int (*cmp)(void *, void *))
{
	int		ret;
	t_node	*node;
	t_node	*curr;
	t_node	*parent;

	curr = *root;
	parent = NULL;
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

bool		remove_node_bst(t_node **node, void *key, int (*cmp)(void *, void *), void *(*min)(t_node *))
{
	t_node	*parent = NULL;
	t_node	*curr = *node;
	t_node	*child;
	void	*successor;

	remove_search_key(&curr, &parent, key, cmp);
	if (curr == NULL)
		return (FAILURE);
	if (curr->left == NULL && curr->right == NULL)
	{
		if (cmp(curr->data, (*node)->data) != 0)
		{
			if (parent->left == curr)
				parent->left = NULL;
			else
				parent->right = NULL;
		}
		else
			*node = NULL;
		del_node(&curr, NULL);
	}
	else if (curr->left && curr->right)
	{
		successor  = min(curr->right);
		remove_node_bst(node, successor, cmp, min);
		curr->data = successor;
	}
	else
	{
		child = (curr->left)? curr->left: curr->right;
		if (curr != *node)
		{
			if (curr == parent->left)
				parent->left = child;
			else
				parent->right = child;
		}
		else
			*node = child;
		del_node(&curr, NULL);
	}
	return (SUCCESS);
}