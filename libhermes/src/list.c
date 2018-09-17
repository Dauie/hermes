#include "../incl/libhermes.h"

t_node *new_node(void) {
	t_node	*node;

	if (!(node = (t_node*)memalloc(sizeof(t_node))))
		return (NULL);
	return (node);
}

void set_node(t_node *node, void *data, size_t size) {
	memcpy(&node->data, &data, size);
}

void			listadd_head(t_node **list, t_node *node)
{
	if (!list || !*list || !node)
		return;
	node->next = *list;
	*list = node;
}

void			listadd_end(t_node **list, t_node *node)
{
	t_node		*tmp;

	if (!list || !*list || !node)
		return;
	tmp = *list;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = node;
}

