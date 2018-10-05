#include "../incl/job.h"
#include "../incl/hermes.h"
#include "../incl/parser.h"

/*TODO need to exclude single IPs from ranges*/

static void	exclude_ip4_ip4(t_targetlist *list, t_node **targets, t_node *exclude)
{
	if (!exclude)
		return ;
	if (exclude->left)
		exclude_ip4_ip4(list, targets, exclude->left);
	if (remove_node_bst(targets, exclude->data, ip4_cmp, ip4_min) == SUCCESS)
	{
		list->ip_cnt--;
		list->total--;
	}
	if (exclude->right)
		exclude_ip4_ip4(list, targets, exclude->right);
}

// BIG TODO : make generic for t_ip4rng && t_prtrng

t_node *split_ip4rng_n(void *data, uint32_t splits)
{
	uint32_t    rem;
	uint32_t    size;
	in_addr_t   start;
	t_node      *node;


	node = new_node(data);
	size = ((t_ip4rng*)data)->size / splits;
	start = ((t_ip4rng*)data)->start;
	while (start < ((t_ip4rng*)data)->end)
	{
        ((t_ip4rng*)node->data)->size = size;
        ((t_ip4rng*)node->data)->start = start;
        ((t_ip4rng*)node->data)->end = start + size;
		add_node_list_head(&node, node->data);
		start = ip4_increment(start, size);
	}
	if ((rem = ((t_ip4rng*)data)->size % splits))
	{
        ((t_ip4rng*)node->data)->size += rem;
        ((t_ip4rng*)node->data)->end  += rem;
	}
	return (node);
}

// BIG TODO : make generic for iprng && portrng

t_node *split_prtrng_n(void *data, uint32_t splits)
{
    uint32_t    rem;
    uint32_t    size;
    in_addr_t   start;
    t_node      *node;


    node = new_node(data);
    size = ((t_prtrng*)data)->size / splits;
    start = ((t_prtrng*)data)->start;
    while (start < ((t_ip4rng*)data)->end)
    {
        ((t_ip4rng*)node->data)->size = size;
        ((t_ip4rng*)node->data)->start = start;
        ((t_ip4rng*)node->data)->end = start + size;
        add_node_list_head(&node, node->data);
        start += size;
    }
    if ((rem = ((t_ip4rng*)data)->size % splits))
    {
        ((t_ip4rng*)node->data)->size += rem;
        ((t_ip4rng*)node->data)->end  += rem;
    }
    return (node);
}

/* Fills 'left' and 'right' with different halves of the range.
 * Returns how many items were removed from the range*/
static int		split_ip4range(t_ip4rng *target, t_ip4rng *exclude,
						  t_ip4rng **l, t_ip4rng **r)
{
	*l = NULL;
	*r = NULL;
	if (ip4_cmp(&exclude->start, &target->start) > 0 &&
			ip4_cmp(&exclude->end, &target->end) < 0)
	{
		*l = new_ip4range();
		*r = new_ip4range();
		(*l)->start = target->start;
		(*l)->end = ip4_decrement(exclude->start, 1);
		(*l)->size = ip4_diff((*l)->start, (*l)->end);
		(*r)->start = ip4_increment(exclude->end, 1);
		(*r)->end = target->end;
		(*r)->size = ip4_diff((*r)->start, (*r)->end);
	}
	else if (ip4_cmp(&exclude->end, &target->start) >= 0 &&
			ip4_cmp(&exclude->end, &target->end) < 0)
	{
		*r = new_ip4range();
		(*r)->start = ip4_increment(exclude->end, 1);
		(*r)->end = target->end;
		(*r)->size = ip4_diff((*r)->start, (*r)->end);
	}
	else if (ip4_cmp(&exclude->start, &target->start) >= 0 &&
			 ip4_cmp(&exclude->start, &target->end) > 0)
	{
		*l = new_ip4range();
		(*l)->start = target->start;
		(*l)->end = ip4_decrement(exclude->start, 1);
		(*l)->size = ip4_diff((*l)->start, (*l)->end);
	}
	return (0);
}

static void		correct_targetlist_totals(t_targetlist *list, t_ip4rng *exclude,
											t_ip4rng *left, t_ip4rng *right)
{
	t_ip4rng		*tmp;


	if (left && right)
	{
		list->total -= exclude->size - (left->size + right->size);
		list->rng_cnt += 1;
	}
	else
	{
		tmp = left ? left : right;
		list->total -= exclude->size - tmp->size;
		list->total -= 1;
	}
}

static void		exclude_ip4_ip4rng(t_targetlist *list, t_node **targets, t_node *exclude)
{
	t_node		*conflict;
	t_ip4rng	*range;
	t_ip4rng	tmp;
	t_ip4rng	*left;
	t_ip4rng	*right;

	if (!exclude)
		return ;
	if (exclude->left)
		exclude_ip4_ip4rng(list, targets, exclude->left);
	if ((conflict = tree_search(targets, exclude->data, ip4_ip4rng_overlap_cmp)))
	{
		tmp.start = ((t_ip4*)exclude->data)->s_addr;
		tmp.end = tmp.start;
		tmp.size = 1;
		range = new_ip4range();
		memcpy(range, conflict->data, sizeof(t_ip4rng));
		remove_node_bst(targets, conflict->data, ip4rng_cmp, ip4rng_min);
		split_ip4range(range, &tmp, &left, &right);
		correct_targetlist_totals(list, range, left, right);
		free(range);
		if (left)
			add_node_bst(targets, (void **) &left, ip4rng_cmp);
		if (right)
			add_node_bst(targets, (void **) &right, ip4rng_cmp);
	}
	if (exclude->right)
		exclude_ip4_ip4rng(list, targets, exclude->right);
}

static void		exclude_ip4rng_ip4rng(t_targetlist *list, t_node **targets,
								  t_node *exclude)
{
	t_node	*conflict;
	t_ip4rng *range;
	t_ip4rng *left;
	t_ip4rng *right;

	if (!exclude)
		return ;
	if (exclude->left)
		exclude_ip4rng_ip4rng(list, targets, exclude->left);
	if ((conflict = tree_search(targets, exclude->data, ip4rng_overlap_cmp)))
	{
		range = new_ip4range();
		memcpy(range, (t_ip4rng*)conflict->data, sizeof(t_ip4rng));
		remove_node_bst(targets, (t_ip4rng*)conflict->data, ip4rng_cmp, ip4rng_min);
		split_ip4range(range, (t_ip4rng*)exclude->data, &left, &right);
		correct_targetlist_totals(list, range, left, right);
		free(range);
		if (left)
			add_node_bst(targets, (void **) &left, ip4rng_cmp);
		if (right)
			add_node_bst(targets, (void **) &right, ip4rng_cmp);
	}
	if (exclude->right)
		exclude_ip4rng_ip4rng(list, targets, exclude->right);
}

static int		do_target_exclusions(t_targetlist *targets, t_targetlist *exclude)
{
	if (!targets || !exclude)
		return (FAILURE);
	if (targets->ips && exclude->ips)
		exclude_ip4_ip4(targets, &targets->ips, exclude->ips);
	if (targets->iprngs && exclude->iprngs)
		exclude_ip4rng_ip4rng(targets, &targets->iprngs, exclude->iprngs);
	if (targets->iprngs && exclude->ips)
		exclude_ip4_ip4rng(targets, &targets->iprngs, exclude->ips);
	return (SUCCESS);
}

static long split_portrange(t_prtrng *target, t_prtrng *exclude,
							t_prtrng **l, t_prtrng **r )
{
	*l = NULL;
	*r = NULL;
	if (port_cmp(&exclude->start, &target->start) > 0 &&
		port_cmp(&exclude->end, &target->end) < 0)
	{
		*l = new_portrange();
		*r = new_portrange();
		(*l)->start = target->start;
		(*l)->end = exclude->start - (uint16_t)1;
		(*l)->size = (*l)->end - (*l)->start;
		(*r)->start = exclude->end + (uint16_t)1;
		(*r)->end = target->end;
		(*r)->size =  (*r)->end - (*r)->start;
	}
	else if (port_cmp(&exclude->end, &target->start) >= 0 &&
			 port_cmp(&exclude->end, &target->end) < 0)
	{
		*r = new_portrange();
		(*r)->start = exclude->end + (uint16_t)1;
		(*r)->end = target->end;
		(*r)->size =  (*r)->end - (*r)->start;
	}
	else if (port_cmp(&exclude->start, &target->start) >= 0 &&
			 port_cmp(&exclude->start, &target->end) > 0)
	{
		*l = new_portrange();
		(*l)->start = target->start;
		(*l)->end = exclude->start - (uint16_t)1;
		(*l)->size = (*l)->end - (*l)->start;
	}
	return (0);
}

static void exclude_port_port(t_portlist *list, t_node **target,
							  t_node *exclude)
{
	if (!exclude)
		return ;
	if (exclude->left)
		exclude_port_port(list, target, exclude->left);
	if (remove_node_bst(target, exclude->data, port_cmp, port_min) == SUCCESS)
	{
		list->port_cnt--;
		list->total--;
	}
	if (exclude->right)
		exclude_port_port(list, target, exclude->right);
}

static void		correct_portrange_totals(t_portlist *list, t_prtrng *exclude,
		t_prtrng *left, t_prtrng *right)
{
	t_prtrng	*tmp;


	if (left && right)
	{
		list->total -= exclude->size - (left->size + right->size);
		list->rng_cnt += 1;
	}
	else
	{
		tmp = left ? left : right;
		list->total -= exclude->size - tmp->size;
		list->total -= 1;
	}
}

static void		exclude_port_prtrng(t_portlist *list, t_node **targets, t_node *exclude)
{
	t_node		*conflict;
	t_prtrng	*range;
	t_prtrng	tmp;
	t_prtrng	*left;
	t_prtrng	*right;

	if (!exclude)
		return ;
	if (exclude->left)
		exclude_port_prtrng(list, targets, exclude->left);
	if ((conflict = tree_search(targets, (t_ip4*)exclude->data, port_prtrng_overlap_cmp)))
	{
		tmp.start = ((t_port*)exclude->data)->port;
		tmp.end = tmp.start;
		tmp.size = 1;
		range = new_portrange();
		memcpy(range, (t_ip4*)conflict->data, sizeof(t_prtrng));
		remove_node_bst(targets, (t_ip4*)conflict->data, portrng_cmp, portrng_min);
		split_portrange(range, &tmp, &left, &right);
		correct_portrange_totals(list, range, left, right);
		free(range);
		if (left)
			add_node_bst(targets, (void **) &left, portrng_cmp);
		if (right)
			add_node_bst(targets, (void **) &right, portrng_cmp);
	}
	if (exclude->right)
		exclude_port_prtrng(list, targets, exclude->right);
}

static void		exclude_prtrng_prtrng(t_portlist *list, t_node **targets,
										 t_node *exclude)
{
	t_node		*conflict;
	t_prtrng	*range;
	t_prtrng	*left;
	t_prtrng	*right;

	if (!exclude)
		return ;
	if (exclude->left)
		exclude_prtrng_prtrng(list, targets, exclude->left);
	if ((conflict = tree_search(targets, exclude->data, portrng_overlap_cmp)))
	{
		range = new_portrange();
		memcpy(range, (t_prtrng*)conflict->data, sizeof(t_prtrng));
		remove_node_bst(targets, (t_prtrng*)conflict->data, portrng_cmp, portrng_min);
		split_portrange(range, (t_prtrng*)exclude->data, &left, &right);
		correct_portrange_totals(list, range, left, right);
		free(range);
		if (left)
			add_node_bst(targets, (void **) &left, portrng_cmp);
		if (right)
			add_node_bst(targets, (void **) &right, portrng_cmp);
	}
	if (exclude->right)
		exclude_prtrng_prtrng(list, targets, exclude->right);
}

static int				do_port_exclusions(t_portlist *target, t_portlist *exclude)
{
	if (!target || !exclude)
		return (FAILURE);
	if (target->ports && exclude->ports)
		exclude_port_port(target, &target->ports, exclude->ports);
	if (target->prtrngs && exclude->prtrngs)
		exclude_prtrng_prtrng(target, &target->prtrngs, exclude->prtrngs);
	if (target->prtrngs && exclude->ports)
		exclude_port_prtrng(target, &target->prtrngs, exclude->ports);
	return (SUCCESS);
}

void			do_exclusions(t_mgr *mgr)
{
	do_target_exclusions(&mgr->job.targets, &mgr->exclude_targets);
	do_port_exclusions(&mgr->job.ports, &mgr->exclude_ports);
}
