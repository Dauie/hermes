#include <stdbool.h>
#include "sys/errno.h"
#include "../incl/libhermes.h"

t_deque			*new_deque(void)
{
	t_deque *deq;

	if (!(deq = (t_deque*)memalloc(sizeof(t_deque))))
		hermes_error(FAILURE, 2, "malloc()", strerror(errno));
	return (deq);
}

bool			enqueue(t_deque **deq, void **data)
{
	t_list *node;

	if (!deq || !*data)
		return (false);
	node = new_node(data);
	if (!*deq)
		*deq = new_deque();
	if (!(*deq)->start && !(*deq)->end)
	{
		(*deq)->start = node;
		(*deq)->end = node;
		node->next = node;
		node->prev = node;
		return (true);
	}
	(*deq)->end->next = node;
	(*deq)->start->prev = node;
	(*deq)->start = node;
	(*deq)->size++;
	if (node > (*deq)->max)
		(*deq)->max = node;
	else if (node < (*deq)->min)
		(*deq)->min = node;
	return (true);
}

bool			push(t_deque **deq, void **data)
{
	t_list *node;

	if (!deq || !data)
		return (false);
	node = new_node(data);
	if (!(*deq)->start && !(*deq)->end)
	{
		(*deq)->start = node;
		(*deq)->end = node;
		node->next = node;
		node->prev = node;
		return (true);
	}
	(*deq)->end->next = node;
	(*deq)->start->prev = node;
	(*deq)->end = node;
	(*deq)->size++;
	if (node > (*deq)->max)
		(*deq)->max = node;
	else if (node < (*deq)->min)
		(*deq)->min = node;
	return (true);
}

void			*peek(t_deque **deq)
{
	if (!deq)
		return (NULL);
	if ((*deq)->end)
		return ((*deq)->end->data);
	return (NULL);
}

t_list			*pop(t_deque **deq)
{
	t_list		*tmp;

	if (!deq)
		return (NULL);
	if (&(*deq)->end == &(*deq)->start)
		return ((*deq)->start);
	tmp = (*deq)->end;
	(*deq)->end = tmp->prev;
	(*deq)->end->next = (*deq)->start;
	(*deq)->start->prev = (*deq)->end;
	return (tmp);
}

void			del_list(t_deque **deq, bool deldata)
{
	t_list		*tmp;

	if (!deq)
		return ;
	tmp = (*deq)->start;
	while (tmp)
	{
		if (deldata && tmp->data)
		{
			free(tmp->data);
			tmp->data = NULL;
		}
		tmp = tmp->next;
		free(tmp->prev);
		tmp->prev = NULL;
	}
}

bool			remove_node_list(t_list **node, bool deldata)
{
	if (!node || !*node)
		return (false);
	if ((*node)->prev)
		(*node)->prev->next = (*node)->next;
	if ((*node)->next)
		(*node)->next->prev = (*node)->prev;
	if (deldata && (*node)->data)
	{
		free((*node)->data);
		(*node)->data = NULL;
	}
	free(*node);
	*node = NULL;
	return (true);
}

bool			remove_list_head(t_list **deq, bool deldata)
{
	t_list		*node;

	if (!deq)
		return (false);
	node = *deq;
	(*deq) = (*deq)->next;
	if (deldata && node->data)
		free(node->data);
	free(node);
	return (true);
}

bool			add_list_head(t_list **deq, void **data)
{
	t_list		*node;

	if (!deq || !data || !*data)
		return (false);
	node = new_node(data);
	if (*deq == NULL)
		*deq = node;
	else
	{
		node->next = *deq;
		*deq = node;
		(*deq)->next->prev = *deq;
	}
	return (true);
}

bool			add_list_end(t_list **deq, void **data)
{
	t_list		*tmp;
	t_list		*node;

	if (!deq || !data || !*data)
		return (false);
	node = new_node(data);
	if (*deq == NULL)
		*deq = node;
	else
	{
		tmp = *deq;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = node;
		node->prev = tmp;
	}
	return (true);
}

#ifdef TESTING
typedef struct  s_data {
	int         value;
}               t_data;

void        print_tree(t_list *tree)
{
	if (tree && tree->data)
	{
		printf("current value -> %i\n", ((t_data*)tree->data)->value);
		print_tree(tree->prev);
		print_tree(tree->next);
	}
	else
		printf("Branch done!\n");
	return;
}

void    *test_min(t_list *tree)
{
	t_data  *save;

	if (!tree)
		return (NULL);
	save = (t_data*)memalloc(sizeof(t_data));
	while (tree->prev)
		tree = tree->prev;
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
	t_list      *node;
	t_list      *tree;
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
