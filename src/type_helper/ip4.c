#include "../../incl/hermes.h"

t_ip4			*new_ip4(void)
{
	t_ip4		*data;

	if (!(data = (t_ip4*)memalloc(sizeof(t_ip4))))
	{
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
		return (NULL);
	}
	return (data);
}

t_ip4rng		*new_ip4range(void)
{
	t_ip4rng	*data;

	if (!(data = (t_ip4rng*)memalloc(sizeof(t_ip4rng))))
	{
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
		return (NULL);
	}
	return (data);
}

t_targetset		*new_targetset(void)
{
	t_targetset *new;

	if (!(new = (t_targetset*)memalloc(sizeof(t_targetset))))
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
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

uint32_t		ip4_rangesize(in_addr_t left, in_addr_t right)
{
	in_addr_t	*l;
	in_addr_t	*r;

	l = &left;
	r = &right;
	if (left < right)
	{
		l = &right;
		r = &left;
	}
	return ((ntohl(*l) - ntohl(*r)) + 1);
}

uint32_t		ip4_diff(in_addr_t left, in_addr_t right)
{
	in_addr_t	*l;
	in_addr_t	*r;

	l = &left;
	r = &right;
	if (left < right)
	{
		l = &right;
		r = &left;
	}
	return (ntohl(*l) - ntohl(*r));
}

in_addr_t		ip4_increment(in_addr_t ip, uint32_t increase)
{
	return (htonl(ntohl(ip) + increase));
}

in_addr_t		ip4_decrement(in_addr_t ip, uint32_t decrease)
{
	return (htonl(ntohl(ip) - decrease));
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
	if (ip4_cmp(&left->start, &right->start) == 0)
	{
		if (ip4_cmp(&left->end, &right->end) < 0)
			return (-1);
		if (ip4_cmp(&left->end, &right->end) > 0)
			return (1);
		else
			return (0);
	}
	else if (ip4_cmp(&left->start, &right->start) > 0)
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
	if (ip4_cmp(&p->s_addr, &rng->start) >= 0 && ip4_cmp(&p->s_addr, &rng->end) <=0)
		return (0);
	return (-1);
}

int				ip4rng_overlap_cmp(void *left, void *right)
{
	t_ip4rng *l;
	t_ip4rng *r;

	l = (t_ip4rng *) left;
	r = (t_ip4rng *) right;
	if (ip4_cmp(&l->start, &r->start) > 0 &&
		ip4_cmp(&l->end, &r->end) < 0)
		return (0);
	else if (ip4_cmp(&l->start, &r->end) <= 0 &&
			 ip4_cmp(&l->end, &r->end) >= 0)
		return (0);
	else if (ip4_cmp(&l->end, &r->start) <= 0 &&
			 ip4_cmp(&l->start, &r->start) >= 0)
		return (0);
	else
		return (-1);
}

void			remove_ip_iprng(t_ip4rng *rng, uint32_t ip, t_ip4rng **l, t_ip4rng **r)
{
	*l = NULL;
	*r = NULL;
	if (ip4_cmp(&rng->start, &ip) == 0)
	{
		rng->size -= 1;
		rng->start = ip4_increment(rng->start, 1);
	}
	else if (ip4_cmp(&rng->end, &ip) == 0)
	{
		rng->size -= 1;
		rng->end = ip4_decrement(rng->end, 1);
	}
	else
	{
		*l = new_ip4range();
		*r = new_ip4range();
		(*l)->start = rng->start;
		(*l)->end = ip4_decrement(ip, 1);
		(*r)->start = ip4_increment(ip, 1);
		(*r)->end = rng->end;
		(*l)->size = ip4_rangesize((*l)->start, (*l)->end);
		(*r)->size = ip4_rangesize((*r)->start, (*r)->end);
	}
}

/* TODO figure out why ip ranges are looping infinitely */
bool			remove_ip_targetset(t_targetset *set, uint32_t ip)
{
	t_node		*seek;
	t_ip4rng	*left;
	t_ip4rng	*right;

	/*look through ranges*/
	seek = set->iprngs;
	do
	{
		if (ip4_ip4rng_overlap_cmp(&ip, seek->data) == 0)
		{
			remove_ip_iprng(seek->data, ip, &left, &right);
			if (left && right)
			{
				clist_rm_head(&seek, true);
				clist_add_head(&seek, (void **)&left);
				clist_add_head(&seek, (void **)&right);
			}
			return (true);
		}
		seek = seek->right;
	}
	while (seek != set->iprngs);
	/* look through ips */
	seek = set->ips;
	do {
		if (ip4_cmp(&ip, seek->data) == 0)
		{
			clist_rm_head(&seek, true);
			return (true);
		}
		seek = seek->right;
	} while (seek != set->ips);
	return (false);
}
