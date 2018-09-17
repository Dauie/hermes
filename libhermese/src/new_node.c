#include "../incl/libhermese.h"

t_node *new_node(void) {
	t_node	*node;

	if (!(node = (t_node*)memalloc(sizeof(t_node))))
		return (NULL);
	return (node);
}

void set_node(t_node *node, void *data, size_t size) {
	memcpy(&node->data, &data, size);
}