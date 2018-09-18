#include "../incl/libhermes.h"
#include "../../incl/job.h"

t_node		*new_node(void)
{
	t_node	*node;

	if (!(node = (t_node*)memalloc(sizeof(t_node))))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	return (node);
}

void			listadd_head(t_node **list, t_node *node)
{
	if (!list || !node)
		return;
	if (*list == NULL)
		*list = node;
	else
	{
		node->next = *list;
		*list = node;
	}
}

void			listadd_end(t_node **list, t_node *node)
{
	t_node		*tmp;

	if (!list || !node)
		return;
	tmp = *list;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = node;
}

