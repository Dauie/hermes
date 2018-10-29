#include <stdbool.h>
#include "sys/errno.h"
#include "../incl/libhermes.h"

bool			clist_add_head(t_node **start, void **data)
{
	t_node		*last;
	t_node		*new;

	if (*start == NULL)
	{
		new = new_node(data);
		new->right = new->left = new;
		*start = new;
		return (true);
	}
	last = (*start)->left;
	new = new_node(data);
	new->right = *start;
	new->left = last;
	last->right = (*start)->left = new;
	*start = new;
	return (true);
}

bool			clist_add_tail(t_node **start, void **data)
{
	t_node		*new;
	t_node		*last;

	if (*start == NULL)
	{
		new = new_node(*data);
		new->right = new->left = new;
		*start = new;
		return (true);
	}
	last = (*start)->left;
	new = new_node(data);
	new->right = *start;
	(*start)->left = new;
	new->right = last;
	last->right = new;
	return (true);
}

bool			clist_rm_head(t_node **clist, bool deldata)
{
	t_node		*tmp;
	t_node		*head;

	if (!clist || !*clist)
		return (false);
	head = *clist;
	if (head->left == head && head->right == head)
	{
		del_node(clist, deldata);
		return (true);
	}
	tmp = head;
	head->left->right = head->right;
	head->right->left = head->left;
	*clist = head->right;
	del_node(&tmp, deldata);
	return (true);
}

bool			clist_rm_tail(t_node **clist, bool deldata)
{
	t_node		*tail;
	t_node		*head;
	t_node		*tmp;

	head = *clist;
	if (!head)
		return (false);
	if (head->left == head && head->right == head)
	{
		del_node(clist, deldata);
		return (true);
	}
	tail = head->left;
	tmp = tail;
	tail->left->right = tail->right;
	tail->right->left = tail->left;
	del_node(&tmp, deldata);
	return (true);
}

bool			clist_rm(t_node **clist, void *data, int (*cmp)(void *, void *))
{
	(void)clist;
	(void)data;
	(void)cmp;
	return (true);
}

void			del_clist(t_node **clist, bool deldata)
{
	t_node		*list;
	t_node		*tmp;

	if (!clist || !*clist)
		return ;
	list = *clist;
	list->left->right = NULL;
	while (list)
	{
		tmp = list;
		list = list->right;
		del_node(&tmp, deldata);
	}
}

bool			rm_node(t_node **node, bool deldata)
{
	t_node		*tmp;

	if (!node || !*node)
		return (false);
	tmp = *node;
	*node = (*node)->right;
	if (tmp->left)
		tmp->left->right = tmp->right;
	if (tmp->right)
		tmp->right->left = tmp->left;
	del_node(&tmp, deldata);
	return (true);
}

void		bst_to_clist_loop(t_node **tree, t_node **clist)
{
	if (!*tree)
		return ;
	if ((*tree)->right)
		bst_to_clist_loop(&(*tree)->right, clist);
	clist_add_head(clist, &(*tree)->data);
	if ((*tree)->left)
		bst_to_clist_loop(&(*tree)->left, clist);
}

t_node		*bst_to_clist(t_node **tree)
{
	t_node	*clist;

	clist = NULL;
	bst_to_clist_loop(tree, &clist);
	del_tree(tree, false);
	return (clist);
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
