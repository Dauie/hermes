#include <stdbool.h>
#include "sys/errno.h"
#include "../incl/libhermes.h"

bool			enqueue(t_node **queue, void **data)
{
	t_node *node;
	t_node *end;

	if (!queue || !data || !*data)
		return (false);
	node = new_node(data);
	if (!*queue)
	{
		*queue = node;
		(*queue)->left = *queue;
		(*queue)->right = *queue;
		return (true);
	}
	end = (*queue)->left;
	(*queue)->left->right = node;
	(*queue)->right->left = node;
	node->left = (*queue)->left;
	node->right = (*queue)->right;
	(*queue) = node;
	return (true);
}

t_node			*dequeue(t_node **queue)
{
	return (*queue);
}


bool			push_stack(t_node **stack, void **data)
{
	t_node		*node;

	if (!stack || !data)
		return (false);
	if (!(node = new_node(data)))
		return (false);
	if (*stack == NULL)
		*stack = node;
	else
	{
		node->right = *stack;
		*stack = node;
		(*stack)->right->left = *stack;
	}
	return (true);
}

void			*pop_stack(t_node **stack)
{
	t_node		*tmp;
	void		*data;

	if (!stack || !*stack)
		return (NULL);
	data = (*stack)->data;
	tmp = *stack;
	*stack = (*stack)->right;
	del_node(&tmp, false);
	return (data);
}

void			del_list(t_node **list, bool deldata)
{
	t_node		*tmp;

	if (!list || !*list)
		return ;
	while (*list)
	{
		tmp = (*list)->right;
		del_node(list, deldata);
		*list = tmp;
	}
}

bool			rm_node(t_node **node, bool deldata)
{
	if (!node || !*node)
		return (false);
	if ((*node)->left)
		(*node)->left->right = (*node)->right;
	if ((*node)->right)
		(*node)->right->left = (*node)->left;
	del_node(node, deldata);
	return (true);
}

bool			rm_list_head(t_node **list, bool deldata)
{
	t_node		*node;

	if (!list || !*list)
		return (false);
	node = *list;
	(*list) = (*list)->right;
	del_node(&node, deldata);
	return (true);
}

bool			add_list_head(t_node **list, void **data)
{
	t_node		*node;

	if (!list || !data || !*data)
		return (false);
	if (!(node = new_node(data)))
		return (false);
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
