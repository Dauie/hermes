#include "../incl/hermes.h"

/* TODO this function probably needs to take a count, to keep all ips from being distributed if total is over a certain size */
void			partition_ip4_tree(t_targetset *src_set, t_node *dst_sets)
{
	t_targetset *set;

	while (dst_sets && src_set->ips)
	{
		set = dst_sets->data;
		if (add_node_bst(&set->ips, src_set->ips->data, ip4_cmp) == true)
		{
			set->ip_cnt += 1;
			set->total += 1;
		}
		if (remove_node_bst(&src_set->ips, src_set->ips->data, ip4_cmp, ip4_min) == true)
		{
			src_set->total -= 1;
			src_set->ips -= 1;
		}
		dst_sets = dst_sets->right;
	}
}

void			partition_ip4rng_tree(t_node **iprngtree, uint32_t parts, t_node *targetsets)
{
	t_targetset	*set;
	t_node		*fragment_lst;
	t_node		*tmp;

	if (!targetsets)
		return ;
	tmp = targetsets;
	fragment_lst = split_ip4rng_n((*iprngtree)->data, parts);
	while (fragment_lst)
	{
		if (!tmp)
			tmp = targetsets;
		set = targetsets->data;
		add_node_bst(&set->iprngs, &fragment_lst->data, ip4rng_cmp);
		/*TODO make a remove_node_head take a bool to delete data or not*/
		remove_list_head(&fragment_lst, 0);
		tmp = tmp->right;
	}
	remove_node_bst(iprngtree, (*iprngtree)->data, ip4rng_cmp, ip4rng_min);
}

t_node			*new_targetset_list(uint32_t count)
{
	t_targetset	*newset;
	t_node		*set_list;

	set_list = NULL;
	while (count)
	{
		newset = new_targetset();
		add_list_head(&set_list, (void **) &newset);
		count--;
	}
	return (set_list);
}

/* TODO: all functions that operate on a targetset's contents should not be
 * TODO: passed the tree, but the targetset itself, to help track its members ^ eg partition_ip4_tree() */
t_node			*partition_targetset(t_targetset *targets, uint32_t parts)
{
	t_node		*targetset_list;

	if (!targets)
		return (NULL);
	if (parts <=1)
		return (new_node((void **)&targets));
	targetset_list = new_targetset_list(parts);
	while (targets->ips)
		partition_ip4_tree(targets, targetset_list);
	while (targets->iprngs)
		partition_ip4rng_tree(NULL, 0, NULL);
	return (targetset_list);
}
