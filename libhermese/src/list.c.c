#include "../incl/libhermese.h"

void			listadd_head(t_node **list, t_node *node)
{
	if (!list || !*list || !node)
		return;
	node->next = *list;
	*list = node;
}

void			listadd_end(t_node **list, t_node *node)
{
	t_list		*tmp;

	if (!list || !*list || !node)
		return;
	tmp = *list;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = node;
}
