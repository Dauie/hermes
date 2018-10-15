#include "../incl/hermes.h"

void			partition_ip4_tree(t_node **ip4tree, t_node *w_targetsets)
{
	t_targetset *set;

	while (w_targetsets && *ip4tree)
	{
		set = w_targetsets->data;
		add_node_bst(&set->ips, &(*ip4tree)->data, ip4_cmp);
		remove_node_bst(ip4tree, (*ip4tree)->data, ip4_cmp, ip4_min);
		w_targetsets = w_targetsets->right;
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

t_node			*partition_targetset(t_targetset *targets, uint32_t parts)
{
	t_node		*targetset_list;

	if (!targets)
		return (NULL);
	if (parts <= 1)
		return (new_node((void **)&targets));
	targetset_list = new_targetset_list(parts);
	while (targets->ips)
		partition_ip4_tree(&targets->ips, targetset_list);
	while (targets->iprngs)
		partition_ip4rng_tree(&targets->iprngs, parts, targetset_list);
	return (targetset_list);
}
