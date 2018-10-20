#include "../incl/libhermes.h"

void	print_tree(t_node **tree, void (*print)(t_node*))
{
	if (!tree || !*tree)
		return ;
	print_tree(&(*tree)->left, print);
	print(*tree);
	print_tree(&(*tree)->right, print);
}
