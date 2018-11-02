#include <stdbool.h>
#include "sys/errno.h"
#include "../incl/libhermes.h"

bool			list_add_head(t_node **list, void **data)
{
	t_node		*new;

	if (!(new = new_node(data)))
		return (false);
	if (*list == NULL)
	{
		*list = new;
		return (true);
	}
	new->right = *list;
	(*list)->left = new;
	*list = new;
	return (true);
}

bool			list_add_tail(t_node **list, void **data)
{
	t_node		*new;
	t_node		*seek;

	new = new_node(data);
	if (*list == NULL)
	{
		*list = new;
		return (true);
	}
	seek = *list;
	while (seek->right)
		seek = seek->right;
	seek->right = new;
	new->left = seek;
	return (true);
}

bool			list_rm_node(t_node **list, t_node **rm, bool deldata)
{
	t_node		*del;

	if (!*list || !*rm)
		return (false);
	del = *list;
	while (del && del != *rm)
		del = del->right;
	if (!del)
		return (false);
	if (del == *list)
		*list = (*list)->right;
	if(del->right != NULL)
		del->right->left = del->left;
	if(del->left != NULL)
		del->left->right = del->right;
	del_node(&del, deldata);
	return (true);
}

void			del_list(t_node **clist, bool deldata)
{
	t_node		*list;
	t_node		*tmp;

	if (!clist || !*clist)
		return ;
	list = *clist;
	while (list)
	{
		tmp = list;
		list = list->right;
		del_node(&tmp, deldata);
	}
	*clist = NULL;
}

void		bst_to_clist_loop(t_node **tree, t_node **clist)
{
	if (!*tree)
		return ;
	if ((*tree)->right)
		bst_to_clist_loop(&(*tree)->right, clist);
	list_add_head(clist, &(*tree)->data);
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
