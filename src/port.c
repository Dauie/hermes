#include "../incl/job.h"

t_port			*new_port()
{
	t_port		*port;

	if (!(port = (t_port *)memalloc(sizeof(t_port))))
		hermes_error(INPUT_ERROR, TRUE, 2, "malloc()", strerror(errno));
	return (port);
}

t_prtrng		*new_portrange()
{
	t_prtrng	*range;

	if (!(range = (t_prtrng *)memalloc(sizeof(t_prtrng))))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	return (range);
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

int				portrng_cmp(void *prt_left, void *prt_right)
{
	t_prtrng	*left;
	t_prtrng	*right;

	left = (t_prtrng*)prt_left;
	right = (t_prtrng*)prt_right;
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

int				port_prtrng_overlap_cmp(void *port, void *prtrng)
{
	t_port		*p;
	t_prtrng *pr;

	p = (t_port*)port;
	pr = (t_prtrng*)prtrng;
	if (p->port >= pr->start && p->port <= pr->end)
		return (0);
	return (-1);
}

int				portrng_overlap_cmp(void *prt_left, void *prt_right)
{
	t_prtrng	*left;
	t_prtrng	*right;

	left = (t_prtrng*)prt_left;
	right = (t_prtrng*)prt_right;
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