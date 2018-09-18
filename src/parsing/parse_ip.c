# include "../../incl/parser.h"

static t_ip4		*new_ip4(void)
{
	t_ip4			*data;

	if (!(data = (t_ip4*)memalloc(sizeof(t_ip4))))
	{
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
		return (NULL);
	}
	return (data);
}

static t_ip4range	*new_ip4range(void)
{
	t_ip4range		*data;

	if (!(data = (t_ip4range*)memalloc(sizeof(t_ip4range))))
	{
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
		return (NULL);
	}
	return (data);
}

static void			set_ip4range(t_ip4range *data, uint32_t ip, uint32_t subn_m)
{
	uint32_t		wildcard;
	uint32_t		netid;

	/* TODO : these might need to be swapped*/
	wildcard = ~subn_m;
	netid = ip & subn_m;
	data->range_size = htonl(wildcard) - 1;
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
	while (cidr_m-- > 0)
	{
		*subn_m |= 0xF;
		*subn_m >>= 1;
	}
	return (SUCCESS);
}

int					parse_ip(uint32_t *ip, char *ip_str)
{
	if (inet_pton(AF_INET, ip_str, ip) <= 0)
		return (hermes_error(INPUT_ERROR, FALSE, 2, "inet_pton()", strerror(errno)));
	return (SUCCESS);
}

static void			add_ip4range(t_node **ip_range, uint32_t ip, uint32_t subn_m)
{
	t_node			*node;
	t_ip4range		*data;

	data = new_ip4range();
	set_ip4range(data, ip, subn_m);
	node = new_node();
	node->data = data;
	listadd_head(ip_range, node);
	return ;
}

static void			add_ip4(t_node **ip_list, uint32_t ip)
{
	t_ip4 			*data;
	t_node			*node;

	data = new_ip4();
	data->addr = ip;
	node = new_node();
	node->data = data;
	listadd_head(ip_list, node);
	return ;
}

int				handle_ip(t_targetlist *targets, char *arg)
{
	char		*ip_str;
	char		*cidr_str;
	uint32_t	ip;
	uint32_t	subn_m;

	if (!arg)
		return (FAILURE);
	cidr_str = arg;
	ip_str = strsep(&cidr_str, "/");
	if (cidr_str != NULL)
	{
		if (parse_ip(&ip, ip_str) < 0)
			return (FAILURE);
		if (parse_cidr_mask(&subn_m, cidr_str) < 0)
			return (FAILURE);
		add_ip4range(&targets->iprange, ip, subn_m);
		targets->iprange_count++;
	}
	else
	{
		if (parse_ip(&ip, arg) < 0)
			return (FAILURE);
		add_ip4(&targets->ip, ip);
		targets->ip_count++;
	}
	return (SUCCESS);
}

#ifdef TESTING
int main(void) {
	int 	error;
	t_node 	*head;
	char 	p_ip[16];
	t_node 	*ip_list;
	char 	input[20];

	ip_list = new_node(sizeof(t_ip4*));
	printf("debugging started > options:\n"
			"<i.p.ad.dr> < test ip\n"
			"display < displays IP list\n"
			"quit/exit/ctrl+c < exits program\n");
	while (1) {
		printf("> ");
		fgets(input, 20, stdin);
		if (!memcmp("display", input, 7)) {
			head = ip_list;
			for (; ip_list->next; ip_list = ip_list->next) {
				inet_ntop(AF_INET, &((t_ip4*)ip_list->data)->addr, p_ip, 16 * sizeof(char));
				printf("%s %i\n", p_ip, ((t_ip4*)ip_list->data)->addr);
			}
			ip_list = head;
		} else if (memcmp("quit", input, 4) == 0 ||
				memcmp("exit", input, 4) == 0) {
				break;
		} else {
			if ((error = parse_ip(input)) < 0)
				printf("parsing failed with %d\n", error);
			add_ip4(&ip_list, error);
		}
		fflush(stdin);
	}
	return (1);
}
#endif