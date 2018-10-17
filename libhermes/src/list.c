#include <stdbool.h>
#include "sys/errno.h"
#include "../incl/libhermes.h"

t_item			*new_item(void *data)
{
	t_item *item;

	if (!(item = (t_item*)memalloc(sizeof(t_item))))
		hermes_error(FAILURE, 2, "malloc()", strerror(errno));
	item->data = data;
	return (item)
}

t_deque			*new_deque(void)
{
	t_deque *deq;

	if (!(deq = (t_deque*)memalloc(sizeof(t_deque))))
		hermes_error(FAILURE, 2, "malloc()", strerror(errno));
	return (deq);
}

bool			enqueue(t_deque **deq, void **data)
{
	t_item *item;

	if (!deq || !*data)
		return (false);
	item = new_item(data);
	if (!*deq)
		*deq = new_deque();
	if (!(*deq)->start && !(*deq)->end)
	{
		(*deq)->start = item;
		(*deq)->end = item;
		item->next = item;
		item->prev = item;
		return (true);
	}
	(*deq)->end->next = item;
	(*deq)->start->prev = item;
	(*deq)->start = item;
	(*deq)->size++;
	if (item > (*deq)->max)
		(*deq)->max = item;
	else if (item < (*deq)->min)
		(*deq)->min = item;
	return (true);
}

bool			push(t_deque **deq, void **data)
{
	t_item *item;

	if (!deq || !data)
		return (false);
	item = new_item(data);
	if (!(*deq)->start && !(*deq)->end)
	{
		(*deq)->start = item;
		(*deq)->end = item;
		item->next = item;
		item->prev = item;
		return (true);
	}
	(*deq)->end->next = item;
	(*deq)->start->prev = item;
	(*deq)->end = item;
	(*deq)->size++;
	if (item > (*deq)->max)
		(*deq)->max = item;
	else if (item < (*deq)->min)
		(*deq)->min = item;
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

t_item			*pop(t_deque **deq)
{
	t_item		*tmp;

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
	t_item		*tmp;

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

bool			remove_node_list(t_slist **item, bool deldata)
{
	if (!item || !*item)
		return (false);
	if ((*item)->prev)
		(*item)->prev->next = (*item)->next;
	if ((*item)->next)
		(*item)->next->prev = (*item)->prev;
	if (deldata && (*item)->data)
	{
		free((*item)->data);
		(*item)->data = NULL;
	}
	free(*item);
	*item = NULL;
	return (true);
}

//bool			remove_list_head(t_list **deq, bool deldata)
//{
//	t_list		*item;
//
//	if (!deq)
//		return (false);
//	item = *deq;
//	(*deq) = (*deq)->next;
//	if (deldata && item->data)
//		free(item->data);
//	free(item);
//	return (true);
//}
//
//bool			add_list_head(t_list **deq, void **data)
//{
//	t_list		*item;
//
//	if (!deq || !data || !*data)
//		return (false);
//	item = new_item(data);
//	if (*deq == NULL)
//		*deq = item;
//	else
//	{
//		item->next = *deq;
//		*deq = item;
//		(*deq)->next->prev = *deq;
//	}
//	return (true);
//}
//
//bool			add_list_end(t_list **deq, void **data)
//{
//	t_list		*tmp;
//	t_list		*item;
//
//	if (!deq || !data || !*data)
//		return (false);
//	item = new_item(data);
//	if (*deq == NULL)
//		*deq = item;
//	else
//	{
//		tmp = *deq;
//		while (tmp->next)
//			tmp = tmp->next;
//		tmp->next = item;
//		item->prev = tmp;
//	}
//	return (true);
//}

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
	t_list      *item;
	t_list      *tree;
	char        input[20];

	tree = new_item();
	tree->data = (t_data*)memalloc(sizeof(t_data));
	while (1) {
		printf("\n> ");
		fgets(input, 20, stdin);
		if (!memcmp("print", input, 5)) {
			print_tree(tree);
		} else if (!memcmp("add item", input, 8)) {
			printf("enter a value: ");
			fgets(input, 20, stdin);
			item = new_item();
			item->data = (t_data*)memalloc(sizeof(t_data));
			((t_data*)item->data)->value = atoi(input);
			add_node(&tree, &item, test_cmp);
		} else if (!memcmp("del item", input, 8)) {
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
