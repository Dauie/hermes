# include "../../incl/parser.h"

t_ip4 *new_ip4(void) {
	t_ip4 *data;

	if (!(data = (t_ip4*)memalloc(sizeof(t_ip4))))
		return (NULL);
	return (data);
}

int verify_ip(uint32_t *ip, char *ip_str) {
	if (inet_pton(AF_INET, ip_str, ip) < 1)
		return (hermes_error(INPUT_ERROR, TRUE, 2, "inet_pton()", strerror(errno)));
	if (*ip > IP_MAX)
		return (hermes_error(INPUT_ERROR, TRUE, 1, "IP out of range"));
	return (1);
}

int get_ip(uint32_t *ip, char *ip_str) {
	printf("%s\n", ip_str);
	if (ip_str == NULL)
		return (FAILURE);
	if (!verify_ip(ip, ip_str))
		return (FAILURE);
	return (SUCCESS);
}

void	set_ip4(t_ip4 *data, uint32_t ip) {
	memcpy(&data->addr, &ip, sizeof(uint32_t));
}

int	get_subnet(uint32_t *subnet, char *cidr) {
	int cidr_m;

	if (!cidr)
		return (FAILURE);
	if ((cidr_m = atoi(cidr)) > 32)
		return (FAILURE);
	if (cidr_m < 0)
		return (FAILURE);
	while (cidr_m-- > 0) {
		*subnet |= MSB;
		*subnet >>= 1;
	}
	return (SUCCESS);
}

void set_ip4range(t_ip4range *data, uint32_t subnet, uint32_t ip) {
	uint32_t start;
	uint32_t end;

	/* TODO : these might need to be swapped*/
	start = subnet | ip;
	end = ~start;

	data->range_size = subnet;
	data->start = start;
	data->end = end;
}

t_ip4range *new_ip4range(void) {
	t_ip4range *data;

	if (!(data = (t_ip4range*)memalloc(sizeof(t_ip4range))))
		return (NULL);
	return (data);
}

uint32_t 	parse_subnet(char *mask) {
	uint32_t 	subnet;

	subnet = 0;
	if (!mask)
		return (FAILURE);
	if (get_subnet(&subnet, mask) < 0)
		return (FAILURE);
	return (subnet);
}

void add_ip4range(t_node ** ip_range, uint32_t subnet, uint32_t ip) {
	t_node		*node;
	t_ip4range 	*data;

	if (!(data = new_ip4range()))
		return;
	if (!(node = new_node(sizeof(t_ip4range))))
		return;
	set_ip4range(data, subnet, ip);
	set_node(node, data, sizeof(data));
	listadd_head(ip_range, node);
	return;
}

void add_ip4(t_node ** ip_list, uint32_t ip) {
	t_ip4 	*data;
	t_node 	*node;

	if (!(data = new_ip4()))
		return;
	if (!(node = new_node(sizeof(t_ip4))))
		return;
	set_ip4(data, ip);
	set_node(node, data, sizeof(data));
	listadd_head(ip_list, node);
	return;
}

uint32_t		parse_ip(char *args) {
	uint32_t	ip;

	if (args == NULL)
		return (FAILURE);
	if (get_ip(&ip, strsep(&args, "\n")) < 0)
		return (FAILURE);
	return (ip);
}

int handle_ip(t_targetlist *targets, char *args) {
	char *ip;
	char *mask;
	uint32_t subnet;
	uint32_t ip_addr;

	mask = args;
	if ((ip = strsep(&mask, "/"))) {
		subnet = parse_subnet(mask);
		add_ip4range(&targets->iprange, subnet, parse_ip(ip));
	} else {
		ip_addr = parse_ip(ip);
		add_ip4(&targets->ip, ip_addr);
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