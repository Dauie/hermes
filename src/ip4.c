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

int				ip4_diff(uint32_t left, uint32_t right)
{
	int diff;

	diff = 0;

	return (diff);
}

void			ip4_add(uint32_t *ip, int increase)
{
	t_ip4bytes	*b;

	if (increase > )
	b = (t_ip4bytes *)ip;


}

void			ip4_subtract(uint32_t *ip, int decrease)
{
	t_ip4bytes	*b;

	b = (t_ip4bytes *)ip;
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

int				ip4rng_overlap_cmp(void *left, void *right)
{
	t_ip4range	*l;
	t_ip4range	*r;

	l = left;
	r = right;
	if (ip4_cmp(&l->start, &r->start) <= 0 && ip4_cmp(&l->end, &r->end) >= 0)
		return (0);/*TODO check logic*/
	else if (ip4_cmp(&l->end, &r->start) >= 0 && ip4_cmp(&l->start, &r->end) <= 0)
		return (0);
	else
		return (-1);
}
