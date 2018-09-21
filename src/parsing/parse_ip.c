# include "../../incl/parser.h"
# include "limits.h"
#ifdef TESTING
t_targetlist		*new_target_list(void);

t_targetlist	*new_target_list(void)
{
	t_targetlist *tlist;

	if (!(tlist = (t_targetlist*)memalloc(sizeof(t_targetlist))))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	return (tlist);
}
#endif

static void			set_ip4range(t_ip4range *data, uint32_t ip, uint32_t subn_m)
{
	uint32_t		wildcard;
	uint32_t		netid;

	wildcard = ~subn_m;
	netid = ip & subn_m;
	data->range_size = htonl(wildcard) - 2;
	data->start = ntohl(htonl(netid) + 1);
	data->end = ntohl(ntohl(netid | wildcard) - 1);
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
	t_node			*node;
	t_ip4range		*data;

	data = new_ip4range();
	set_ip4range(data, ip, subn_m);
	node = new_node();
	node->data = data;
	return (add_node(ip_range, &node, ip4range_cmp));
}

static int			bstadd_ip4(t_node **ip_list, uint32_t ip)
{
	t_ip4 			*data;
	t_node			*node;

	data = new_ip4();
	data->addr = ip;
	node = new_node();
	node->data = data;
	return (add_node(ip_list, &node, ip4_cmp));
}

int				do_ip4range(t_targetlist *targets, char *ip_str, char *cidr_str)
{
	uint32_t	ip;
	uint32_t	subn_m;

	if (parse_ip(&ip, ip_str) < 0)
		return (FAILURE);
	if (parse_cidr_mask(&subn_m, cidr_str) < 0)
		return (FAILURE);
	if (bstadd_ip4range(&targets->iprange, ip, subn_m) == SUCCESS)
	{
		targets->iprange_count++;
		targets->total += ((t_ip4range*)targets->iprange->data)->range_size;
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
		targets->ip_count++;
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

#ifdef TESTING
int main(void) {
	int 	error;
	t_node 	*head;
	char 	p_ip[16];
	t_targetlist 	*target_list;
	char 	input[20];

	target_list = new_target_list();
	printf("debugging started > options:\n"
			"<i.p.ad.dr> < test ip\n"
			"display < displays IP list\n"
			"quit/exit/ctrl+c < exits program\n");
	while (1) {
		printf("> ");
		fgets(input, 20, stdin);
		if (!memcmp("display ip", input, 10)) {
			head = target_list->ip;
			for (; target_list->ip; target_list->ip = target_list->ip->next) {
				inet_ntop(AF_INET, &((t_ip4*)target_list->ip->data)->addr, p_ip, 16 * sizeof(char));
				printf("address		:%s\n", p_ip);
			}
			target_list->ip = head;
		} else if (!memcmp("display range", input, 13)) {
			for (; target_list->iprange; target_list->iprange = target_list->iprange->next) {
				inet_ntop(AF_INET, &((t_ip4range*)target_list->iprange->data)->range_size, p_ip, 16 * sizeof(char));
				printf("range size 	: %s\n", p_ip);
				inet_ntop(AF_INET, &((t_ip4range*)target_list->iprange->data)->start, p_ip, 16 * sizeof(char));
				printf("start		:%s\n", p_ip);
				inet_ntop(AF_INET, &((t_ip4range*)target_list->iprange->data)->end, p_ip, 16 * sizeof(char));
				printf("end			:%s\n", p_ip);
			}
			target_list->iprange = head;
		} else if (memcmp("quit", input, 4) == 0 ||
			memcmp("exit", input, 4) == 0) {
			break;
		} else {
			error = handle_ip(&target_list, input);
			printf("parsing ended with %d\n", error);
		}
		fflush(stdin);
	}
	return (1);
}
#endif