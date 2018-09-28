#include "../incl/job.h"
#include "../incl/hermes.h"

t_ip4			*new_ip4(void)
{
	t_ip4		*data;

	if (!(data = (t_ip4*)memalloc(sizeof(t_ip4))))
	{
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
		return (NULL);
	}
	return (data);
}

t_ip4range		*new_ip4range(void)
{
	t_ip4range	*data;

	if (!(data = (t_ip4range*)memalloc(sizeof(t_ip4range))))
	{
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
		return (NULL);
	}
	return (data);
}

int			ip4_cmp(void *left, void *right)
{
	int shift;
	uint32_t l;
	uint32_t r;

	l = ((t_ip4*)left)->addr;
	r = ((t_ip4*)right)->addr;
	shift = 0xFF;
	while (shift)
	{
		if ((l & shift) < (r & shift))
			return (-1);
		else if ((l & shift) > (r & shift))
			return (1);
		shift <<= 8;
	}
	return (0);
}

uint32_t			ip4_diff(uint32_t left, uint32_t right)
{
	uint32_t		diff;
	long long		ret;
	t_ip4bytes		*l;
	t_ip4bytes		*r;

	diff = 0;
	l = (t_ip4bytes *)&left;
	r = (t_ip4bytes *)&right;
	ret = l->b1 - r->b1;
	if (ret < 0)
		ret *= -1;
	diff += ret * (255 * 255 * 255);
	ret = l->b2 - r->b2;
	if (ret < 0)
		ret *= -1;
	diff += ret * (255 * 255);
	ret = l->b3 - r->b3;
	if (ret < 0)
		ret *= -1;
	diff += ret * 255;
	ret = l->b4 - r->b4;
	if (ret < 0)
		ret *= -1;
	diff += ret;
	return (diff);
}

/*TODO: do this without looping, its quick and dirty right now*/
uint32_t		ip4_increment(uint32_t ip, int increase)
{
	t_ip4bytes *b;

	b = (t_ip4bytes *)&ip;
	while (increase--)
	{
		if (b->b4 < 255)
			b->b4 += 1;
		else
		{
			b->b4 = 0;
			if (b->b3 < 255)
				b->b3 += 1;
			else
			{
				b->b3 = 0;
				if (b->b2 < 255)
					b->b2 += 1;
				else
				{
					b->b2 = 0;
					if (b->b1 < 255)
						b->b1 += 1;
					else
						b->b1 = 0;
				}
			}
		}
	}
	return (ip);
}

/*TODO: do this without looping, its quick and dirty right now*/
uint32_t			ip4_decrement(uint32_t ip, int decrease)
{
	t_ip4bytes *b;

	b = (t_ip4bytes *)&ip;
	while (decrease--)
	{
		if (b->b4 > 0)
			b->b4 -= 1;
		else
		{
			b->b4 = 255;
			if (b->b3 > 0)
				b->b3 -= 1;
			else
			{
				b->b3 = 255;
				if (b->b2 > 0)
					b->b2 -= 1;
				else
				{
					b->b2 = 255;
					if (b->b1 > 0)
						b->b1 -= 1;
					else
						b->b1 = 0;
				}
			}
		}
	}
	return (ip);
}

void			*ip4_min(t_node *tree)
{
	t_ip4		*save;

	if (!tree)
		return (NULL);
	save = new_ip4();
	while (tree->left)
		tree = tree->left;
	memcpy(save, tree->data, sizeof(t_ip4));
	return (save);
}

int				ip4rng_cmp(void *ipr_left, void *ipr_right)
{
	t_ip4range	*left;
	t_ip4range	*right;

	left = ipr_left;
	right = ipr_right;
	if (left->start == right->start)
	{
		if (left->end < right->end)
			return (-1);
		if (left->end > right->end)
			return (1);
		else
			return (0);
	}
	else if (left->start > right->start)
		return (1);
	else
		return (-1);
}

void			*ip4rng_min(t_node *tree)
{
	t_ip4range	*save;

	if (!tree)
		return (NULL);
	save = new_ip4range();
	while (tree->left)
		tree = tree->left;
	memcpy(save, tree->data, sizeof(t_ip4range));
	return (save);
}

/*TODO replace if statements with whats in split range*/
int				ip4rng_overlap_cmp(void *left, void *right)
{
	t_ip4range	*l;
	t_ip4range	*r;

	l = left;
	r = right;
	if (ip4_cmp(&l->start, &r->start) > 0 &&
		ip4_cmp(&l->end, &r->end) < 0)
		return (0);
	else if (ip4_cmp(&l->end, &r->start) >= 0 &&
			 ip4_cmp(&l->end, &r->end) < 0)
		return (0);
	else if (ip4_cmp(&l->start, &r->start) >= 0 &&
			 ip4_cmp(&l->start, &r->end) > 0)
		return (0);
	else
		return (-1);
}
