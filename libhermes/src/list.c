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

int		add_node(t_node **root, t_node **node, int (*cmp)(void *, void *))
{
	int ret;
	t_node *curr = *root;
	t_node *parent = NULL;

	if (*root == NULL)
	{
		*root = *node;
		return (SUCCESS);
	}
	while (curr != NULL)
	{
		parent = curr;
		ret = cmp((*node)->data, curr->data);
		if (ret < 0)
			curr = curr->left;
		else if (ret > 0)
			curr = curr->right;
		else
		{
			del_node(node);
			return (FAILURE);
		}
	}
	if (cmp((*node)->data, parent->data) < 0)
		parent->left = *node;
	else
		parent->right = *node;
	return (SUCCESS);
}

void search_key(t_node **curr, t_node **parent, void *key, int (*cmp)(void *, void *))
{
	while (curr != NULL && cmp((*curr)->data, key) != 0)
	{
		*parent = *curr;
		if (cmp(key, (*curr)->data) < 0)
			*curr = (*curr)->left;
		else
			*curr = (*curr)->right;
	}
}

int			remove_node(t_node **root, void *key, int (*cmp)(void *, void *), void *(*min)(t_node *))
{
	t_node* parent = NULL;
	t_node *curr = *root;
	t_node *child;
	void	*successor;

	search_key(&curr, &parent, key, cmp);
	if (curr == NULL)
		return (FAILURE);
	if (curr->left == NULL && curr->right == NULL)
	{
		if (cmp(curr->data, (*root)->data) != 0)
		{
			if (parent->left == curr)
				parent->left = NULL;
			else
				parent->right = NULL;
		}
		else
			*root = NULL;
		del_node(&curr);
	}
	else if (curr->left && curr->right)
	{
		successor  = min(curr->right);
		remove_node(root, successor, cmp, min);
		curr->data = successor;
	}
	else
	{
		child = (curr->left)? curr->left: curr->right;
		if (curr != *root)
		{
			if (curr == parent->left)
				parent->left = child;
			else
				parent->right = child;
		}
		else
			*root = child;
		del_node(&curr);
	}
	return (SUCCESS);
}



#ifdef TESTING
typedef struct  s_data {
	int         value;
}               t_data;

void        print_tree(t_node *tree)
{
	if (tree && tree->data)
	{
		printf("current value -> %i\n", ((t_data*)tree->data)->value);
		print_tree(tree->left);
		print_tree(tree->right);
	}
	else
		printf("Branch done!\n");
	return;
}

void    *test_min(t_node *tree)
{
	t_data  *save;

	if (!tree)
		return (NULL);
	save = (t_data*)memalloc(sizeof(t_data));
	while (tree->left)
		tree = tree->left;
	memcpy(save, tree->data, sizeof(t_data));
	return (save);
}

int     test_cmp(void *l, void* r)
{
	return ((((t_data*)l)->value < ((t_data*)r)->value) ? -1:
			(((t_data*)l)->value > ((t_data*)r)->value) ? 1:0);
}

int     main(void)
{
	t_data      *data;
	t_node      *node;
	t_node      *tree;
	char        input[20];

	tree = new_node();
	tree->data = (t_data*)memalloc(sizeof(t_data));
	while (1) {
		printf("\n> ");
		fgets(input, 20, stdin);
		if (!memcmp("print", input, 5)) {
			print_tree(tree);
		} else if (!memcmp("add node", input, 8)) {
			printf("enter a value: ");
			fgets(input, 20, stdin);
			node = new_node();
			node->data = (t_data*)memalloc(sizeof(t_data));
			((t_data*)node->data)->value = atoi(input);
			add_node(&tree, &node, test_cmp);
		} else if (!memcmp("del node", input, 8)) {
			printf("enter a value: ");
			fgets(input, 20, stdin);
			data = (t_data*)malloc(sizeof(t_data));
			data->value = atoi(input);
			remove_node(&tree, data, test_min, test_cmp);
		} else if (!memcmp("del tree", input, 8)) {
			delete_tree(&tree);
		} else if (!memcmp("quit", input, 4) ||
				!memcmp("exit", input, 4)) {
			break;
		} else {
			printf("Invalid command");
		}
		fflush(stdin);
	}
	return (1);
	return (0);
}
#endif

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
