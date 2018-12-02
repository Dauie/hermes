#include "../incl/hermes.h"
#include "../incl/binnify.h"
#include "../incl/message.h"

void	distribute_obj(t_node **wrkr_tree, uint8_t type, binn *obj)
{
	t_wrkr *wrkr;

	if ((*wrkr_tree)->left)
		distribute_obj(&(*wrkr_tree)->left, type, obj);

	wrkr = (*wrkr_tree)->data;
	(void)wrkr;

	if ((*wrkr_tree)->right)
		distribute_obj(&(*wrkr_tree)->right, type, obj);
}
