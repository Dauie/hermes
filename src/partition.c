#include "../incl/hermes.h"
#include "../incl/binnify.h"
#include "../incl/message.h"


/* TODO this function probably needs to take a count, to keep all ips from being distributed if total is over a certain size */
uint32_t		partition_ip4(t_node **dst, t_node **src,
							  uint32_t amt)
{
	t_node *head;

	if (!dst || !amt)
		return (amt);
	head = (*src);
	do
	{

		head = head->right;
	} while (head != (*src))
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
			return (0);
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

/* TODO: all functions that operate on a targetset's contents should not be
 * TODO: passed the tree, but the targetset itself, to help track its members ^ eg partition_ip4() */
uint32_t partition_targetset(t_targetset *dst, t_targetset *src, uint32_t amt)
{
	uint32_t	parts;

	if (!src || !amt || !dst)
		return (0);
	parts = amt;
	dst->ip_cnt = amt;
	dst->total = amt;
	if (!(parts -= partition_ip4(&dst->ips, &src->ips, amt / 2)))
		return (amt);
	dst->rng_cnt = amt;
	partition_ip4rng(&dst->iprngs, &src->iprngs, parts);
	amt -= parts;
	return (amt);
}