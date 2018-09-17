# include "../../incl/parser.h"

t_ip4 *new_ip4(void) {
	t_ip4 *data;

	if (!(data = (t_ip4*)memalloc(sizeof(t_ip4))))
		return (NULL);
	return (data);
}

int verify_ip(uint32_t *ip, char *ip_str) {
	if (inet_pton(AF_INET, ip_str, ip) < 1)
		return (0);
	if (*ip > IP_MAX)
		return (0);
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
	printf("%i\n", data->addr);
	memcpy(&data->addr, &ip, sizeof(uint32_t));
	printf("%i\n", data->addr);
}

long	get_cidr(char *cidr) {
	int cidr_m;
	long result;

	result = 0;
	if (!cidr)
		return (FAILURE);
	if ((cidr_m = atoi(cidr)) > 32)
		return (FAILURE);
	if (cidr_m < 0)
		return (FAILURE);
	while (cidr_m-- > 0) {
		result |= 2147483648;
		result >>= 1;
	}
	return (result);
}

int		parse_ip(t_node **ip_list, char *args) {
	uint32_t	ip;
	t_ip4		*data;
	t_node		*node;
	long		cidr_m;

	if (args == NULL)
		return (FAILURE);
	if (!(data = new_ip4()))
		return (FAILURE);
	if (!(node = new_node()))
		return (FAILURE);
	if (get_ip(&ip, strsep(&args, "\n")) < 0)
		return (FAILURE);
	set_ip4(data, ip);
	set_node(node, &data, sizeof(data));

	#ifdef TESTING
	#endif

	listadd_head(ip_list, &node);
	return (SUCCESS);
}

#ifdef TESTING
int main(void) {
	char 	*p_ip;
	int 	error;
	t_node 	*head;
	t_node 	*ip_list;
	char 	input[20];

	p_ip = NULL;
	ip_list = new_node();
	printf("debugging started > options:\n"
			"<i.p.ad.dr> < test ip\n"
			"display < displays IP list\n"
			"quit/exit/ctrl+c < exits program\n");
	while (1) {
		printf("> ");
		fgets(input, 20, stdin);
		if (!memcmp("display", input, 7)) {
			head = ip_list;
			for (; ip_list->data; ip_list = ip_list->next) {
				inet_ntop(AF_INET, &((t_ip4*)ip_list->data)->addr, p_ip, INET_ADDRSTRLEN);
				printf("%s\n", p_ip);
			}
			ip_list = head;
		} else if (memcmp("quit", input, 4) == 0 ||
				memcmp("exit", input, 4) == 0) {
				break;
		} else {
			if ((error = parse_ip(&ip_list, input)) < 0)
				printf("parsing failed with %d\n", error);
		}
		fflush(stdin);
	}
	return (1);
}
#endif