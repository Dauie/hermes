#include "../incl/libhermes.h"

void print_depth(t_node *tree, int level, void (*print)(t_node*))
{
	if (tree == NULL)
		return;
	if (level == 1)
		print(tree);
	else if (level > 1)
	{
		print_depth(tree->left, level - 1, print);
		print_depth(tree->right, level - 1, print);
	}
}

int 	tree_height(t_node *tree)
{
	int l;
	int r;
	
	if (!tree)
		return (0);
	else
	{
		l = tree_height(tree->left);
		r = tree_height(tree->right);
		if (l > r)
			return(l + 1);
		else
			return(r + 1);
	}
}

void	print_tree(t_node *tree, void (*print)(t_node*))
{
	int h;
	int i;

	i = 0;
	h = tree_height(tree);
	printf("tree has depth of: %d\n", h);
	while (++i <= h) {
		print_depth(tree, i, print);
		printf("\n");
	}
}

//void	print_tree(t_node **tree, void (*print)(t_node*))
//{
//	if (!tree || !*tree)
//		return ;
//	print_tree(&(*tree)->left, print);
//	print(*tree);
//	print_tree(&(*tree)->right, print);
//}
