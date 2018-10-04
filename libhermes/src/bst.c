# include "sys/errno.h"
# include "../incl/libhermes.h"

t_bst		*new_node_bst(void)
{
	t_bst	*node;

	if (!(node = (t_bst*)memalloc(sizeof(t_bst))))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	return (node);
}

void		del_bst_node(t_bst **node)
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

t_bst		*bst_search(t_bst **tree, void *data, int (*cmp)(void *, void *))
{
	t_bst	*cur;

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

t_bst		*tree_search(t_bst **tree, void *data, int (*cmp)(void *, void *))
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

int			add_node_bst(t_bst **root, void **data, int (*cmp)(void *, void *))
{
	int ret;
	t_bst *node;
	t_bst *curr = *root;
	t_bst *parent = NULL;

	node = new_node_bst();
	node->data = *data;
	if (*root == NULL)
	{
		*root = node;
		return (SUCCESS);
	}
	while (curr != NULL)
	{
		parent = curr;
		ret = cmp(data, curr->data);
		if (ret < 0)
			curr = curr->left;
		else if (ret > 0)
			curr = curr->right;
		else
		{
			del_bst_node(&node);
			return (FAILURE);
		}
	}
	if (cmp(data, parent->data) < 0)
		parent->left = node;
	else
		parent->right = node;
	return (SUCCESS);
}

void		remove_search_key(t_bst **curr, t_bst **parent, void *key,
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

int			remove_node_bst(t_bst **tree, void *key,
							   int (*cmp)(void *, void *),
							   void *(*min)(t_bst *))
{
	t_bst* parent = NULL;
	t_bst *curr = *tree;
	t_bst *child;
	void	*successor;

	remove_search_key(&curr, &parent, key, cmp);
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
		del_bst_node(&curr);
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
		del_bst_node(&curr);
	}
	return (SUCCESS);
}