#include "../incl/libhermes.h"

t_node					*new_node(void* data)
{
	t_node		*node;

	if (!(node = (t_node*)memalloc(sizeof(t_node))))
	{
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
		return (NULL);
	}
	node->data = data;
	return (node);
}
