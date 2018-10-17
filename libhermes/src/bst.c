# include "sys/errno.h"
# include "../incl/libhermes.h"

t_list		*bst_search(t_list **tree, void *data, int (*cmp)(void *, void *))
{
	t_list	*cur;

	cur = *tree;
	while (cur)
	{
		if (cmp(cur->data, data) < 0)
			cur = cur->prev;
		else if (cmp(cur->data, data) > 0)
			cur = cur->next;
		else
			return (cur);
	}
	return (NULL);
}

t_list		*tree_search(t_list **tree, void *data, int (*cmp)(void *, void *))
{
	if (!*tree)
		return (NULL);
	if ((*tree)->prev)
		return (tree_search(&(*tree)->prev, data, cmp));
	if (cmp(data, (*tree)->data) == 0)
		return (*tree);
	if ((*tree)->next)
		return (tree_search(&(*tree)->prev, data, cmp));
	return (NULL);
}

bool		add_node_bst(t_list **root, void **data, int (*cmp)(void *, void *))
{
	int		ret;
	t_list	*node;
	t_list	*curr = *root;
	t_list	*parent = NULL;

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
			curr = curr->prev;
		else if (ret > 0)
			curr = curr->next;
		else
		{
			del_node(&node, NULL);
			return (false);
		}
	}
	if (cmp(data, parent->data) < 0)
		parent->prev = node;
	else
		parent->next = node;
	return (true);
}

void		remove_search_key(t_list **curr, t_list **parent, void *key,
					int (*cmp)(void *, void *))
{
	while (*curr != NULL && cmp((*curr)->data, key) != 0)
	{
		*parent = *curr;
		if (cmp(key, (*curr)->data) < 0)
			*curr = (*curr)->prev;
		else
			*curr = (*curr)->next;
	}
}

bool		remove_node_bst(t_list **tree, void *key, int (*cmp)(void *, void *), void *(*min)(t_list *))
{
	t_list	*parent = NULL;
	t_list	*curr = *tree;
	t_list	*child;
	void	*successor;

	remove_search_key(&curr, &parent, key, cmp);
	if (curr == NULL)
		return (FAILURE);
	if (curr->prev == NULL && curr->next == NULL)
	{
		if (cmp(curr->data, (*tree)->data) != 0)
		{
			if (parent->prev == curr)
				parent->prev = NULL;
			else
				parent->next = NULL;
		}
		else
			*tree = NULL;
		del_node(&curr, NULL);
	}
	else if (curr->prev && curr->next)
	{
		successor  = min(curr->next);
		remove_node_bst(tree, successor, cmp, min);
		curr->data = successor;
	}
	else
	{
		child = (curr->prev)? curr->prev: curr->next;
		if (curr != *tree)
		{
			if (curr == parent->prev)
				parent->prev = child;
			else
				parent->next = child;
		}
		else
			*tree = child;
		del_node(&curr, NULL);
	}
	return (SUCCESS);
}