# include "limits.h"
# include "../../incl/hermes.h"

static void			set_ip4range(t_ip4rng *data, uint32_t ip, uint32_t subn_m)
{
	uint32_t		wildcard;
	uint32_t		netid;

	wildcard = ~subn_m;
	netid = ip & subn_m;
	if (subn_m != 0xFFFFFFFF)
	{
		data->size = htonl(wildcard) - 1;
		data->start = ntohl(htonl(netid) + 1);
		data->end = ntohl(ntohl(netid | wildcard) - 1);
	}
	else
	{
		data->size = 1;
		data->start = netid;
		data->end = data->start;
	}
}

static int			parse_cidr_mask(uint32_t *subn_m, char *cidr_str)
{
	int				cidr_m;

	if (!cidr_str)
		return (hermes_error(INPUT_ERROR, FALSE, 1, "no cidr mask provided"));
	if ((cidr_m = atoi(cidr_str)) > 32 || cidr_m < 0)
		return (hermes_error(INPUT_ERROR, FALSE, 2, "bad cidr mask:", cidr_str));
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

int					parse_ip(uint32_t *ip, char *ip_str)
{
	if (inet_pton(AF_INET, ip_str, ip) <= 0)
		return (hermes_error(INPUT_ERROR, FALSE, 2, "inet_pton()", strerror(errno)));
	return (SUCCESS);
}

static int			bstadd_ip4range(t_node **ip_range, uint32_t ip,
									   uint32_t subn_m)
{
	t_ip4rng		*data;

	data = new_ip4range();
	set_ip4range(data, ip, subn_m);
	return (add_node(ip_range, (void **)&data, ip4rng_cmp));
}

static int			bstadd_ip4(t_node **ip_list, uint32_t ip)
{
	t_ip4 			*data;

	data = new_ip4();
	data->addr = ip;
	return (add_node(ip_list, (void **)&data, ip4_cmp));
}

int				do_ip4range(t_targetlist *targets, char *ip_str, char *cidr_str)
{
	uint32_t	ip;
	uint32_t	subn_m;

	if (parse_ip(&ip, ip_str) < 0)
		return (FAILURE);
	if (parse_cidr_mask(&subn_m, cidr_str) < 0)
		return (FAILURE);
	if (bstadd_ip4range(&targets->iprng, ip, subn_m) == SUCCESS)
	{
		targets->rng_cnt++;
		targets->total += ((t_ip4rng*)targets->iprng->data)->size;
	}
	return (SUCCESS);
}

int				do_ip4(t_targetlist *targets, char *input)
{
	uint32_t	ip;

	if (parse_ip(&ip, input) < 0)
		return (FAILURE);
	if (bstadd_ip4(&targets->ip, ip) == SUCCESS)
	{
		targets->ip_cnt++;
		targets->total++;
	}
	return (SUCCESS);
}

int				handle_ip(t_targetlist *targets, char *input)
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
