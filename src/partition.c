#include "../incl/hermes.h"

/* TODO this function probably needs to take a count, to keep all ips from being distributed if total is over a certain size */
uint32_t		partition_ip4(t_node **dst, t_node **src,
							  uint32_t amt)
{
	if (!dst || !amt)
		return (amt);
	partition_ip4(dst, &(*src)->left, amt);
	if (add_node_bst(dst, &(*src)->data, ip4_cmp) == true)
		amt--;
	partition_ip4(dst, &(*src)->right, amt);
	return (amt);
}

uint32_t		partition_ip4rng(t_node **dst, t_node **src,
								 uint32_t amt)
{
	t_ip4rng *slice;

	if (!dst || !amt)
		return (amt);
	if (((t_ip4rng*)(*src)->data)->size > amt)
	{
		if (!(slice = slice_ip4rng(src, amt)))
			hermes_error(SUCCESS, 1, "no src or amt provided for slice_ip4rng(src, amt)");
		if (add_node_bst(&(*dst), (void**)slice, ip4rng_cmp) == true)
			amt--;
	}
	else
	{
		if (add_node_bst(dst, (*src)->data, ip4rng_cmp))
			amt--;
	}
	return (amt);
}

t_node			*new_targetset_list(uint32_t count)
{
	t_targetset	*newset;
	t_node		*set_list;

	set_list = NULL;
	while (count)
	{
		newset = new_targetset();
		enqueue(&set_list, (void*)&newset);
		count--;
	}
	return (set_list);
}

/* TODO: all functions that operate on a targetset's contents should not be
 * TODO: passed the tree, but the targetset itself, to help track its members ^ eg partition_ip4() */
void partition_targetset(t_targetset **dst, t_targetset **src, uint32_t amt)
{
	if (!src || !amt || !dst)
		return;
	(*dst)->ip_cnt = amt;
	(*dst)->total = amt;
	if (!(amt = partition_ip4(&(*dst)->ips, &(*src)->ips, amt / 2)))
		return;
	(*dst)->rng_cnt = amt;
	partition_ip4rng(&(*dst)->iprngs, &(*src)->iprngs, amt);
}
