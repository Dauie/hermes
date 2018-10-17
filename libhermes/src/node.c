#include "../incl/libhermes.h"

t_list			*new_node(void **data)
{
	t_list		*node;

	if (!(node = (t_list*)memalloc(sizeof(t_list))))
	{
		hermes_error(errno, true, 2, "malloc()", strerror(errno));
		return (NULL);
	}
	node->data = *data;
	return (node);
}

void			del_node(t_list **node, bool deldata)
{
	if (!node || !*node)
		return ;
	if (deldata && (*node)->data)
	{
		free((*node)->data);
		(*node)->data = NULL;
	}
	free(*node);
	*node = NULL;
}

