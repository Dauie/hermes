#include "../incl/libhermes.h"

t_node			*new_node(void **data)
{
	t_node		*node;

	if (!(node = (t_node*)memalloc(sizeof(t_node))))
	{
		hermes_error(errno, true, 2, "malloc()", strerror(errno));
		return (NULL);
	}
	node->data = *data;
	return (node);
}

void			del_node(t_node **node, bool deldata)
{
	if (!node || !*node)
		return ;
	if (deldata && (*node)->data)
	{
		free((*node)->data);
		(*node)->data = NULL;
	}
	(*node)->left = NULL;
	(*node)->right = NULL;
	free(*node);
	*node = NULL;
}

