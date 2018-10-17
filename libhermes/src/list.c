#include <stdbool.h>
#include "sys/errno.h"
#include "../incl/libhermes.h"

bool			enqueue(t_node **list, void **data)
{
	t_node *node;

	if (!list || !data)
		return (false);
	node = new_node(data);
	if (!(*list)->left && !(*list)->right)
	{
		(*list) = node;
		(*list)->left = node;
		(*list)->right = node;
		return (true);
	}
	(*list)->left->left = node;
	node->right = (*list)->left;
	(*list)->left = node;
	return (true);
}

bool			push(t_node **list, void **data)
{
	t_node *node;

	if (!list || !data)
		return (false);
	node = new_node(data);
	if (!(*list)->left && !(*list)->right)
	{
		(*list)->left = node;
		(*list)->right = node;
		return (true);
	}
	(*list)->right->right = node;
	node->left = (*list)->right;
	(*list)->right = node;
	return (true);
}

void			*peek(t_node **list)
{
	if (!list)
		return (NULL);
	if ((*list)->right)
		return ((*list)->right->data);
	return (NULL);
}

t_node			*pop(t_node **list)
{
	t_node		*tmp;

	if (!list)
		return (NULL);
	if (&(*list)->left == &(*list)->right)
		return (*list);
	tmp = (*list)->right;
	(*list)->right = (*list)->right->left;
	return (tmp);
}

void			del_list(t_node **list, bool deldata)
{
	t_node		*tmp;

	if (!list)
		return ;
	while (*list)
	{
		tmp = (*list)->right;
		if (deldata && (*list)->data)
		{
			free((*list)->data);
			(*list)->data = NULL;
		}
		free(*list);
		*list = NULL;
		*list = tmp;
	}
}

bool			remove_node_list(t_node **node, bool deldata)
{
	if (!node || !*node)
		return (false);
	if ((*node)->left)
		(*node)->left->right = (*node)->right;
	if ((*node)->right)
		(*node)->right->left = (*node)->left;
	if (deldata && (*node)->data)
	{
		free((*node)->data);
		(*node)->data = NULL;
	}
	free(*node);
	*node = NULL;
	return (true);
}

bool			remove_list_head(t_node **list, bool deldata)
{
	t_node		*node;

	if (!list)
		return (false);
	node = *list;
	(*list) = (*list)->right;
	if (deldata && node->data)
		free(node->data);
	free(node);
	return (true);
}

bool			add_list_head(t_node **list, void **data)
{
	t_node		*node;

	if (!list || !data || !*data)
		return (false);
	node = new_node(data);
	if (*list == NULL)
		*list = node;
	else
	{
		node->right = *list;
		*list = node;
		(*list)->right->left = *list;
	}
	return (true);
}

bool			add_list_end(t_node **list, void **data)
{
	t_node		*tmp;
	t_node		*node;

	if (!list || !data || !*data)
		return (false);
	node = new_node(data);
	if (*list == NULL)
		*list = node;
	else
	{
		tmp = *list;
		while (tmp->right)
			tmp = tmp->right;
		tmp->right = node;
		node->left = tmp;
	}
	return (true);
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
