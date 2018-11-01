#include "../incl/hermes.h"
#include "../incl/binnify.h"
#include "../incl/message.h"

t_ip4rng		*slice_ip4rng(t_ip4rng *src, uint32_t amt)
{
	t_ip4rng	*dst;

	if (!src || amt <= 0)
		return (NULL);
	dst = new_ip4range();
	dst->size = amt;
	dst->start = src->start;
	dst->end = ip4_increment(src->start, amt - 1);
	src->size -= amt;
	src->start = ip4_increment(src->start, amt);
	return (dst);
}

void			 transfer_work(t_targetset *dst, t_targetset *src, uint32_t reqamt)
{
	t_ip4rng	*slice;
	uint32_t	ipcnt;
	t_ip4rng	rng;

	if (!src || reqamt <= 0)
		return ;
	if (reqamt > src->total)
		reqamt = src->total;
	if (reqamt > 2)
		ipcnt = reqamt / 2;
	else
		ipcnt = reqamt;
	reqamt -= ipcnt;
	while (ipcnt && src->ip_cnt > 0 && src->ips)
	{
		if (list_add_head(&dst->ips, &src->ips->data) == true)
		{
			dst->total += 1;
			dst->ip_cnt += 1;
		}
		else
			break;
		if (list_rm_node(&src->ips, false) == true)
		{
			src->total -= 1;
			src->ip_cnt -= 1;
		}
		ipcnt--;
	}
	reqamt += ipcnt;
	while (reqamt && src->total > 0 && src->iprngs)
	{
		memcpy(&rng, src->iprngs->data, sizeof(t_ip4rng));
		if (rng.size <= reqamt)
		{
			if (list_add_head(&dst->iprngs, &src->iprngs->data) == true)
			{
				dst->total += rng.size;
				dst->rng_cnt += 1;
			}
			else
				break;
			if (list_rm_node(&src->iprngs, false) == true)
			{
				src->total -= rng.size;
				src->rng_cnt -= 1;
			}
			reqamt -= rng.size;
		}
		else
		{
			if (!(slice = slice_ip4rng(src->iprngs->data, reqamt)))
			{
				hermes_error(FAILURE, "slice_ip4rng() failed");
				break;
			}
			src->total -= slice->size;
			list_add_head(&dst->iprngs, (void **) &slice);
			reqamt -= slice->size;
			dst->rng_cnt += 1;
			dst->total += slice->size;
		}
	}
}
