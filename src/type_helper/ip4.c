#include "../../incl/job.h"
#include "../../incl/hermes.h"

t_ip4			*new_ip4(void)
{
	t_ip4		*data;

	if (!(data = (t_ip4*)memalloc(sizeof(t_ip4))))
	{
		hermes_error(FAILURE, 2, "malloc()", strerror(errno));
		return (NULL);
	}
	return (data);
}

t_ip4rng		*new_ip4range(void)
{
	t_ip4rng	*data;

	if (!(data = (t_ip4rng*)memalloc(sizeof(t_ip4rng))))
	{
		hermes_error(FAILURE, 2, "malloc()", strerror(errno));
		return (NULL);
	}
	return (data);
}

t_targetset		*new_targetset(void)
{
	t_targetset *new;

	if (!(new = (t_targetset *)memalloc(sizeof(t_targetset))))
		hermes_error(FAILURE, 2, "malloc()", strerror(errno));
	return (new);
}


int				ip4_cmp(void *left, void *right)
{
	uint32_t	shift;
	in_addr_t	l;
	in_addr_t	r;

	l = ((t_ip4*)left)->s_addr;
	r = ((t_ip4*)right)->s_addr;
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

uint32_t			ip4_diff(in_addr_t left, in_addr_t right)
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
in_addr_t		ip4_increment(in_addr_t ip, uint32_t increase)
{
	t_ip4bytes	*b;

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
in_addr_t		ip4_decrement(in_addr_t ip, uint32_t decrease)
{
	t_ip4bytes	*b;

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
						b->b1 = 255;
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
	t_ip4rng	*left;
	t_ip4rng	*right;

	left = (t_ip4rng*)ipr_left;
	right = (t_ip4rng*)ipr_right;
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
	t_ip4rng	*save;

	if (!tree)
		return (NULL);
	save = new_ip4range();
	while (tree->left)
		tree = tree->left;
	memcpy(save, tree->data, sizeof(t_ip4rng));
	return (save);
}

int				ip4_ip4rng_overlap_cmp(void *ip, void *iprng)
{
	t_ip4		*p;
	t_ip4rng	*rng;

	p = (t_ip4*)ip;
	rng = (t_ip4rng*)iprng;
	if (p->s_addr >= rng->start && p->s_addr <= rng->end)
		return (0);
	return (-1);
}

int				ip4rng_overlap_cmp(void *left, void *right)
{
	t_ip4rng	*l;
	t_ip4rng	*r;

	l = (t_ip4rng*)left;
	r = (t_ip4rng*)right;
	if (ip4_cmp(&l->start, &r->start) >= 0 &&
		ip4_cmp(&l->end, &r->end) <= 0)
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
