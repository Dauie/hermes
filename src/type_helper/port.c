# include "../../incl/hermes.h"

t_port			*new_port()
{
	t_port		*port;

	if (!(port = (t_port *)memalloc(sizeof(t_port))))
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
	return (port);
}

t_prtrng		*new_portrange()
{
	t_prtrng	*range;

	if (!(range = (t_prtrng *)memalloc(sizeof(t_prtrng))))
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
	return (range);
}

t_portset		*new_portset()
{
	t_portset	*set;

	if (!(set = (t_portset *)memalloc(sizeof(t_portset))))
		hermes_error(FAILURE, "malloc() %s", strerror(errno));;
	return (set);
}
int				port_cmp(void *prt_left, void *prt_right)
{
	t_port		*left;
	t_port		*right;

	left = (t_port*)prt_left;
	right = (t_port*)prt_right;
	if (left->port == right->port)
		return (0);
	else if (left->port > right->port)
		return (1);
	else
		return (-1);
}

void			*port_min(t_node *tree)
{
	t_port		*save;

	if (!tree)
		return (NULL);
	save = new_port();
	while (tree->left)
		tree = tree->left;
	memcpy(save, tree->data, sizeof(t_ip4));
	return (save);
}

int				portrng_cmp(void *rng_left, void *rng_right)
{
	t_prtrng	*left;
	t_prtrng	*right;

	left = (t_prtrng*)rng_left;
	right = (t_prtrng*)rng_right;
	if (left->start == right->start)
	{
		if (left->end < right->end)
			return (-1);
		else if (left->end > right->end)
			return (1);
		else
			return (0);
	}
	else if (left->start > right->start)
		return (1);
	else
		return (-1);
}

int				port_prtrng_overlap_cmp(void *prt, void *rng)
{
	t_port		*p;
	t_prtrng *pr;

	p = (t_port*)prt;
	pr = (t_prtrng*)rng;
	if (p->port >= pr->start && p->port <= pr->end)
		return (0);
	return (-1);
}

int				portrng_overlap_cmp(void *rng_left, void *rng_right)
{
	t_prtrng	*left;
	t_prtrng	*right;

	left = (t_prtrng*)rng_left;
	right = (t_prtrng*)rng_right;
	if (left->start >= right->start && left->start <= right->end)
		return (0);
	else if (left->end >= right->start && left->end <= right->end)
		return (0);
	else
		return (-1);

}

void			*portrng_min(t_node *tree)
{
	t_prtrng	*save;

	if (!tree)
		return (NULL);
	save = new_portrange();
	while (tree->left)
		tree = tree->left;
	memcpy(save, tree->data, sizeof(t_ip4rng));
	return (save);
}