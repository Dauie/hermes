#include "../incl/hermes.h"

/* TODO this function probably needs to take a count, to keep all ips from being distributed if total is over a certain size */
uint32_t		partition_ip4(t_targetset **dst, t_targetset **src,
							  uint32_t amt)
{
	uint32_t portion;

	portion = amt;
	while ((*src)->ips && amt)
	{
		if (add_list_head(&(*dst)->ips, &(*src)->ips->data) == true)
		{
			(*dst)->ip_cnt++;
			(*dst)->total++;
		}
		if (rm_node_bst(&(*dst)->ips, (*dst)->ips->data, ip4_cmp, ip4_min) == true)
		{
			(*src)->ip_cnt--;
			(*src)->total--;
		}
		amt--;
	}
	portion -= amt;
	return (portion);
}

uint32_t		partition_ip4rng(t_targetset **dst, t_targetset **src,
								 uint32_t amt)
{
	t_ip4rng	*slice;
	uint32_t	portion;

	portion = amt;
	while ((*src)->iprngs && amt)
	{
		if (IP4RNG_DATA((*src))->size > amt)
		{
			if (!(slice = slice_ip4rng(src, amt)))
				hermes_error(SUCCESS, 1, "no src or amt provided for slice_ip4rng(src, amt)");
			if (add_list_head(&(*dst)->iprngs, (void**)slice) == true)
			{
				(*dst)->ip_cnt += amt;
				(*dst)->total++;
			}
		}
		else
		{
			if (add_list_head(&(*src)->iprngs, (*src)->iprngs->data))
			{
				(*dst)->ip_cnt += IP4RNG_DATA((*dst))->size;
				(*dst)->total++;
			}
			if (rm_node_bst(&(*src)->iprngs, (*src)->iprngs->data, ip4rng_cmp,
							ip4rng_min) == true)
			{
				(*src)->ip_cnt--;
				(*src)->total--;
			}
		}
	}
	portion -= amt;
	return (portion);
}

/* TODO: all functions that operate on a targetset's contents should not be
 * TODO: passed the tree, but the targetset itself, to help track its members ^ eg partition_ip4() */
void 		partition_targetset(t_targetset **dst, t_targetset **src, uint32_t amt)
{
	if (!src || !amt || !dst)
		return;
	if (!(amt -= partition_ip4(dst, src, amt / 2)))
		return;
	partition_ip4rng(dst, src, amt);
}
