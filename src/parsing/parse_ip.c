# include "../../incl/hermes.h"

void			set_ip4range(t_ip4rng *data, const in_addr_t *ip, const in_addr_t *subn_m)
{
	in_addr_t		wildcard;
	in_addr_t		netid;

	wildcard = ~(*subn_m);
	netid = (*ip) & (*subn_m);
	if (*subn_m != 0xFFFFFFFF)
	{
		data->size = htonl(wildcard) + 1;
		data->start = ntohl(htonl(netid));
		data->end = ntohl(ntohl(netid | wildcard));
	}
	else
	{
		data->size = 1;
		data->start = netid;
		data->end = data->start;
	}
}

int			parse_cidr_mask(in_addr_t *subn_m, char *cidr_str)
{
	int				cidr_m;

	if (!cidr_str)
		return (hermes_error(FAILURE, "no cidr mask provided"));
	if ((cidr_m = atoi(cidr_str)) > 32 || cidr_m < 0)
		return (hermes_error(FAILURE, "bad cidr mask: %s", cidr_str));
	*subn_m = 0;
	if (cidr_m > 0)
	{
		*subn_m = 0x80000000;
		while (cidr_m-- > 1)
		{
			*subn_m >>=  1;
			*subn_m |= 0x80000000;
		}
	}
	*subn_m = ntohl(*subn_m);
	return (SUCCESS);
}

int					parse_ip(in_addr_t *ip, char *ip_str)
{
	if (inet_pton(AF_INET, ip_str, ip) <= 0)
		return (hermes_error(FAILURE, "inet_pton() %s", strerror(errno)));
	return (SUCCESS);
}

int				do_ip4range(t_targetset *targets, char *ip_str, char *cidr_str)
{
	in_addr_t	ip;
	in_addr_t	subn_m;
	t_ip4rng	*data;


	if (parse_ip(&ip, ip_str) < 0)
		return (FAILURE);
	if (parse_cidr_mask(&subn_m, cidr_str) < 0)
		return (FAILURE);
	data = new_ip4range();
	set_ip4range(data, &ip, &subn_m);
	if (add_node_bst(&targets->iprngs, (void **) &data, ip4rng_cmp))
	{
		targets->rng_cnt++;
		targets->total += ((t_ip4rng*)targets->iprngs->data)->size;
	}
	return (SUCCESS);
}

int				do_ip4(t_targetset *targets, char *input)
{
	in_addr_t	ip;
	t_ip4		*data;


	if (parse_ip(&ip, input) < 0)
		return (FAILURE);
	data = new_ip4();
	data->s_addr = ip;
	if (add_node_bst(&targets->ips, (void **) &data, ip4_cmp))
	{
		targets->ip_cnt++;
		targets->total++;
	}
	return (SUCCESS);
}

int				handle_ip(t_targetset *targets, char *input)
{
	char		*section;
	char		*ip_str;
	char		*cidr_str;

	if (!input)
		return (FAILURE);
	while ((section = strsep(&input, ",")))
	{
		cidr_str = section;
		ip_str = strsep(&cidr_str, "/");
		if (cidr_str != NULL)
		{
			if (do_ip4range(targets, ip_str, cidr_str) == FAILURE)
				return (FAILURE);
		}
		else
			if (do_ip4(targets, section) == FAILURE)
				return (FAILURE);
	}
	return (SUCCESS);
}
