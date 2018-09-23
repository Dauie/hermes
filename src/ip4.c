#include "../incl/job.h"
#include "../incl/hermes.h"

int test_cmp(uint32_t l, uint32_t r);

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

int ip4_cmp(uint32_t l, uint32_t r)
{
	int ret;
	int shift;

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
/* TODO ip comparison need to be done byte by byte, instead of a full 32bit int*/
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

int				ip4rng_overlap_cmp(void *prt_left, void *prt_right)
{
	t_ip4range	*left;
	t_ip4range	*right;

	left = prt_left;
	right = prt_right;
	if (test_cmp(left->start, right->start) <= 0 && test_cmp(left->end, right->end) >= 0)
		return (0);
	else if (test_cmp(left->end, right->start) >= 0 && test_cmp(left->end, right->end) <= 0)
		return (0);
	else
		return (-1);
}

//int main(void)
//{
//	t_targetlist	*list;
//	uint32_t		left_addr;
//	uint32_t		right_addr;
//	t_node			*targets;
//	t_node			*exclude;
//
//	left_addr = 2570;
//	right_addr = 16779968;
//	exclude_ip4range();
//    return (0);
//}
//#endif