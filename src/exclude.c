#include "../incl/job.h"
#include "../incl/hermes.h"
#include "../incl/parser.h"

/*TODO need to exclude single IPs from ranges*/

void	exclude_ip4(t_targetlist *list, t_node **targets, t_node *exclude)
{
	if (!exclude)
		return ;
	if (exclude->left)
		exclude_ip4(list, targets, exclude->left);
	if (remove_node(targets, exclude->data, ip4_cmp, ip4_min) == SUCCESS)
	{
		list->ip_count--;
		list->total--;
	}
	if (exclude->right)
		exclude_ip4(list, targets, exclude->right);
}

/* Fills 'left' and 'right' with different halves of the range.
 * Returns how many items were removed from the range*/
int		split_ip4range(t_ip4range *target, t_ip4range *exclude,
						  t_ip4range **l, t_ip4range **r)
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

static void		correct_targetlist_totals(t_targetlist *list, t_ip4range *exclude,
											t_ip4range *left, t_ip4range *right)
{
	t_ip4range		*tmp;


	if (left && right)
	{
		list->total -= exclude->size - (left->size + right->size);
		list->iprange_count += 1;
	}
	else
	{
		tmp = left ? left : right;
		list->total -= exclude->size - tmp->size;
		list->total -= 1;
	}
}

/*TODO: handle totals after split/removal */
void		exclude_ip4range(t_targetlist *list, t_node **targets, t_node *exclude)
{
	t_node	*conflict;
	t_ip4range *range;
	t_ip4range *left;
	t_ip4range *right;

	if (!exclude)
		return ;
	if (exclude->left)
		exclude_ip4range(list, targets, exclude->left);
	if ((conflict = tree_search(targets, exclude->data, ip4rng_overlap_cmp)))
	{
		range = new_ip4range();
		memcpy(range, conflict->data, sizeof(t_ip4range));
		remove_node(targets, conflict->data, ip4rng_cmp, ip4rng_min);
		split_ip4range(range, exclude->data, &left, &right);
		correct_targetlist_totals(list, range, left, right);
		free(range);
		if (left)
			add_node(targets, (void **)&left, ip4rng_cmp);
		if (right)
			add_node(targets, (void **)&right, ip4rng_cmp);
	}
	if (exclude->right)
		exclude_ip4range(list, targets, exclude->right);
}

int		do_target_exclusions(t_targetlist *targets, t_targetlist *exclude)
{
	if (!targets || !exclude)
		return (FAILURE);
	if (targets->ip && exclude->ip)
		exclude_ip4(targets, &targets->ip, exclude->ip);
	if (targets->iprange && exclude->iprange)
		exclude_ip4range(targets, &targets->iprange, exclude->iprange);
	return (SUCCESS);
}

static long split_portrange(t_portrange *target, t_portrange *exclude,
							t_portrange **l, t_portrange **r )
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

static void exclude_ports(t_portlist *list, t_node **target, t_node *exclude)
{
	if (!exclude)
		return ;
	if (exclude->left)
		exclude_ports(list, target, exclude->left);
	if (remove_node(target, exclude->data, port_cmp, port_min) == SUCCESS)
	{
		list->port_count--;
		list->total--;
	}
	if (exclude->right)
		exclude_ports(list, target, exclude->right);
}

static void		correct_portrange_totals(t_portlist *list, t_portrange *exclude,
		t_portrange *left, t_portrange *right)
{
	t_portrange		*tmp;


	if (left && right)
	{
		list->total -= exclude->size - (left->size + right->size);
		list->range_count += 1;
	}
	else
	{
		tmp = left ? left : right;
		list->total -= exclude->size - tmp->size;
		list->total -= 1;
	}
}

static void		exclude_portrange(t_portlist *list, t_node **targets, t_node *exclude)
{
	t_node		*conflict;
	t_portrange	*range;
	t_portrange	*left;
	t_portrange	*right;

	if (!exclude)
		return ;
	if (exclude->left)
		exclude_portrange(list, targets, exclude->left);
	if ((conflict = tree_search(targets, exclude->data, portrng_overlap_cmp)))
	{
		range = new_portrange();
		memcpy(range, conflict->data, sizeof(t_portrange));
		remove_node(targets, conflict->data, portrng_cmp, portrng_min);
		split_portrange(range, exclude->data, &left, &right);
		correct_portrange_totals(list, range, left, right);
		free(range);
		if (left)
			add_node(targets, (void **)&left, portrng_cmp);
		if (right)
			add_node(targets, (void **)&right, portrng_cmp);
	}
	if (exclude->right)
		exclude_portrange(list, targets, exclude->right);
}

int		do_port_exclusions(t_portlist *target, t_portlist *exclude)
{
	if (!target || !exclude)
		return (FAILURE);
	if (target->ports && exclude->ports)
		exclude_ports(target, &target->ports, exclude->ports);
	if (target->port_range && exclude->port_range)
		exclude_portrange(target, &target->port_range, exclude->port_range);
	return (SUCCESS);
}

