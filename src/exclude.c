#include "../incl/hermes.h"

t_ip4rng		*slice_ip4rng(t_targetset **src, uint32_t amt)
{
	t_ip4rng *iprng;

	if (!src || !amt)
		return (NULL);
	iprng = new_ip4range();
	iprng->size = amt;
	iprng->start = ((t_ip4rng*)(*src)->iprngs->data)->start;
	iprng->end = BYTORD_SUM(((t_ip4rng*)(*src)->iprngs->data)->end, -amt);
	((t_ip4rng*)(*src)->iprngs->data)->start = BYTORD_SUM(iprng->end, 1);
	return (iprng);
}

static void		exclude_ip4_ip4(t_targetset *set, t_node **targets, t_node *exclude)
{
	if (!exclude)
		return ;
	if (exclude->left)
		exclude_ip4_ip4(set, targets, exclude->left);
	if (remove_node_bst(targets, exclude->data, ip4_cmp, ip4_min) == true)
	{
		set->ip_cnt--;
		set->total--;
	}
	if (exclude->right)
		exclude_ip4_ip4(set, targets, exclude->right);
}

t_node			*split_ip4rng_portions(t_ip4rng *data, uint32_t splits)
{
	uint32_t	rem;
	uint32_t	end;
	uint32_t	size;
	uint32_t	start;
	t_node		*node;
	t_ip4rng	*iprng;

	node = NULL;
	iprng = NULL;
	size = data->size / splits;
	start = ntohl(data->start);
	end = ntohl(data->end);
	while ((start + size) < end)
	{
		iprng = new_ip4range();
		iprng->size = size;
		iprng->start = htonl(start);
		iprng->end = htonl(start + size - 1);
		add_list_end(&node, (void**)&iprng);
		start += size;
	}
	if ((rem = (data->size % splits)))
	{
		iprng = new_ip4range();
		iprng->size = rem;
		iprng->start = htonl(start);
		iprng->end = htonl(start + rem - 1);
		add_list_end(&node, (void**)&iprng);
	}
	return (node);
}

static int		split_ip4range(t_ip4rng *target, t_ip4rng *exclude, t_ip4rng **l, t_ip4rng **r)
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
	return (SUCCESS);
}

static void		correct_targetset_totals(t_targetset *set, t_ip4rng *exclude, t_ip4rng *left, t_ip4rng *right)
{
	t_ip4rng	*tmp;

	if (left && right)
	{
		set->total -= exclude->size - (left->size + right->size);
		set->rng_cnt += 1;
	}
	else
	{
		tmp = left ? left : right;
		set->total -= exclude->size - tmp->size;
		set->total -= 1;
	}
}

static void		exclude_ip4_ip4rng(t_targetset *set, t_node **targets, t_node *exclude)
{
	t_node		*conflict;
	t_ip4rng	*range;
	t_ip4rng	tmp;
	t_ip4rng	*left;
	t_ip4rng	*right;

	if (!exclude)
		return ;
	if (exclude->left)
		exclude_ip4_ip4rng(set, targets, exclude->left);
	if ((conflict = tree_search(targets, exclude->data, ip4_ip4rng_overlap_cmp)))
	{
		tmp.start = ((t_ip4*)exclude->data)->s_addr;
		tmp.end = tmp.start;
		tmp.size = 1;
		range = new_ip4range();
		memcpy(range, conflict->data, sizeof(t_ip4rng));
		remove_node_bst(targets, conflict->data, ip4rng_cmp, ip4rng_min);
		split_ip4range(range, &tmp, &left, &right);
		correct_targetset_totals(set, range, left, right);
		free(range);
		if (left)
			add_node_bst(targets, (void **) &left, ip4rng_cmp);
		if (right)
			add_node_bst(targets, (void **) &right, ip4rng_cmp);
	}
	if (exclude->right)
		exclude_ip4_ip4rng(set, targets, exclude->right);
}

static void		exclude_ip4rng_ip4rng(t_targetset *set, t_node **targets, t_node *exclude)
{
	t_node		*conflict;
	t_ip4rng	*range;
	t_ip4rng	*left;
	t_ip4rng	*right;

	if (!exclude)
		return ;
	if (exclude->left)
		exclude_ip4rng_ip4rng(set, targets, exclude->left);
	if ((conflict = tree_search(targets, exclude->data, ip4rng_overlap_cmp)))
	{
		range = new_ip4range();
		memcpy(range, (t_ip4rng*)conflict->data, sizeof(t_ip4rng));
		remove_node_bst(targets, (t_ip4rng*)conflict->data, ip4rng_cmp, ip4rng_min);
		split_ip4range(range, (t_ip4rng*)exclude->data, &left, &right);
		correct_targetset_totals(set, range, left, right);
		free(range);
		if (left)
			add_node_bst(targets, (void **) &left, ip4rng_cmp);
		if (right)
			add_node_bst(targets, (void **) &right, ip4rng_cmp);
	}
	if (exclude->right)
		exclude_ip4rng_ip4rng(set, targets, exclude->right);
}

static int		do_target_exclusions(t_targetset *targets, t_targetset *exclude)
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

static int split_portrange(t_prtrng *target, t_prtrng *exclude,
						   t_prtrng **l, t_prtrng **r)
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
	return (SUCCESS);
}

static void		exclude_port_port(t_portset *set, t_node **target, t_node *exclude)
{
	if (!exclude)
		return ;
	if (exclude->left)
		exclude_port_port(set, target, exclude->left);
	if (remove_node_bst(target, exclude->data, port_cmp, port_min) == true)
	{
		set->port_cnt--;
		set->total--;
	}
	if (exclude->right)
		exclude_port_port(set, target, exclude->right);
}

static void		correct_portrange_totals(t_portset *set, t_prtrng *exclude,
		t_prtrng *left, t_prtrng *right)
{
	t_prtrng	*tmp;


	if (left && right)
	{
		set->total -= exclude->size - (left->size + right->size);
		set->rng_cnt += 1;
	}
	else
	{
		tmp = left ? left : right;
		set->total -= exclude->size - tmp->size;
		set->total -= 1;
	}
}

static void		exclude_port_prtrng(t_portset *set, t_node **targets, t_node *exclude)
{
	t_node		*conflict;
	t_prtrng	*range;
	t_prtrng	tmp;
	t_prtrng	*left;
	t_prtrng	*right;

	if (!exclude)
		return ;
	if (exclude->left)
		exclude_port_prtrng(set, targets, exclude->left);
	if ((conflict = tree_search(targets, (t_ip4*)exclude->data, port_prtrng_overlap_cmp)))
	{
		tmp.start = ((t_port*)exclude->data)->port;
		tmp.end = tmp.start;
		tmp.size = 1;
		range = new_portrange();
		memcpy(range, (t_ip4*)conflict->data, sizeof(t_prtrng));
		remove_node_bst(targets, (t_ip4*)conflict->data, portrng_cmp, portrng_min);
		split_portrange(range, &tmp, &left, &right);
		correct_portrange_totals(set, range, left, right);
		free(range);
		if (left)
			add_node_bst(targets, (void **) &left, portrng_cmp);
		if (right)
			add_node_bst(targets, (void **) &right, portrng_cmp);
	}
	if (exclude->right)
		exclude_port_prtrng(set, targets, exclude->right);
}

static void		exclude_prtrng_prtrng(t_portset *set, t_node **targets,
		t_node *exclude)
{
	t_node		*conflict;
	t_prtrng	*range;
	t_prtrng	*left;
	t_prtrng	*right;

	if (!exclude)
		return ;
	if (exclude->left)
		exclude_prtrng_prtrng(set, targets, exclude->left);
	if ((conflict = tree_search(targets, exclude->data, portrng_overlap_cmp)))
	{
		range = new_portrange();
		memcpy(range, (t_prtrng*)conflict->data, sizeof(t_prtrng));
		remove_node_bst(targets, (t_prtrng*)conflict->data, portrng_cmp, portrng_min);
		split_portrange(range, (t_prtrng*)exclude->data, &left, &right);
		correct_portrange_totals(set, range, left, right);
		free(range);
		if (left)
			add_node_bst(targets, (void **) &left, portrng_cmp);
		if (right)
			add_node_bst(targets, (void **) &right, portrng_cmp);
	}
	if (exclude->right)
		exclude_prtrng_prtrng(set, targets, exclude->right);
}

static int		do_port_exclusions(t_portset *target, t_portset *exclude)
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
	do_target_exclusions(mgr->job.targets, mgr->exclude_targets);
	do_port_exclusions(mgr->job.ports, mgr->exclude_ports);
}
