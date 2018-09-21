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


int				add_node(t_node **tree, t_node **node,
							int (*cmp)(void *, void *))
{
	t_node		*cur;
	t_node		*prnt;

	if (!node)
		return (FAILURE);
	if (!*tree)
	{
		*tree = *node;
		return (SUCCESS);
	}
	prnt = cur = *tree;
	while (cur)
	{
		prnt = cur;
		if (cmp(cur->data, (*node)->data) < 0)
			cur = cur->left;
		else if (cmp(cur->data, (*node)->data) > 0)
			cur = cur->right;
		/* Keep duplicate nodes from being added*/
		else
		{
			del_node(node);
			return (FAILURE);
		}
	}
	if (cmp(prnt->data, (*node)->data) < 0)
		prnt->left = *node;
	else
		prnt->right = *node;
	return (SUCCESS);
}

static t_node			*bst_rm_search(t_node **tree, t_node **parent,
								void *data, int (*cmp)(void *, void *))
{
	t_node		*cur;
	t_node		*prnt;

	cur = *tree;
	while (cur)
	{
		prnt = cur;
		if (cmp(cur->data, data) < 0)
			cur = cur->left;
		else if (cmp(cur->data, data) > 0)
			cur = cur->right;
		else
		{
			*parent = prnt;
			return (cur);
		}
	}
	*parent = NULL;
	return (NULL);
}

int			remove_node(t_node **tree, void *data, void *(*min)(t_node *),
						int (*cmp)(void *, void *))
{
	t_node		*curr;
	t_node		*prnt;
	t_node		*chld;
	void		*save;

	prnt = NULL;
	if (!(curr = bst_rm_search(tree, &prnt, data, cmp)))
		return (FAILURE);
	if (!curr->left && !curr->right)
	{
		if (curr != *tree)
		{
			if (prnt->left == curr)
				prnt->left = NULL;
			else
				prnt->right = NULL;
		}
		else
			*tree = NULL;
		del_node(&curr);
	}
	else if (curr->left && curr->right)
	{
		save = min(curr->right);
		remove_node(&curr, save, min, cmp);
		curr->data = save;
	}
	else
	{
		chld = curr->left ? curr->left : curr->right;
		if (curr != *tree)
		{
			if (curr == prnt->left)
				prnt->left = chld;
			else
				prnt->right = chld;
		}
		else
			*tree = chld;
		del_node(&curr);
	}
	return (SUCCESS);
}

void		delete_tree(t_node **tree)
{
	if (tree && *tree)
	{
        return;
	}
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