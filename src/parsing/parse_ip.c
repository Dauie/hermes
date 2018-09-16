#include <arpa/inet.h>
#include "../../incl/parser.h"
#include "../../incl/defined.h"
#include "../../libhermese/incl/libhermese.h"

int verify_ip(uint32_t *ptr, char *ip) {
	if (!inet_pton(AF_INET, ip, ptr))
		return (-1);
	if (*ptr <= 0 || *ptr >= IP_MAX)
		return (-1);
	return (0);
}

t_ip4 *new_ip4(void) {
	t_ip4 *data;

	if (!(data = (t_ip4 *) memalloc(sizeof(t_ip4))))
		return (NULL);
	return (data);
}

uint32_t get_ip(char *ip_str) {
	uint32_t ip_addr;

	if (ip_str == NULL)
		return (FAILURE);
	if (verify_ip(&ip_addr, ip_str) < 0)
		return (FAILURE);
	return (ip_addr);
}

void set_ip4(t_ip4 *data, uint32_t ip, uint32_t cidr) {
	memcpy(&data->addr, &ip, sizeof(uint32_t));
	memcpy(&data->cidr, &cidr, sizeof(uint32_t));
}

uint32_t get_cidr(char *cidr) {
	int cidr_m;
	uint32_t result;

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

int parse_ip(t_node *ip_list, char *args) {
	uint32_t ip;
	char *cidr;
	t_ip4 *data;
	t_node *node;
	uint32_t cidr_m;

	if (args == NULL)
		return (FAILURE);
	if (!(data = new_ip4()))
		return (FAILURE);
	if (!(node = new_node()))
		return (FAILURE);
	cidr = memchr(args, '/', strlen(args));
	if (!(ip = get_ip(args)))
		return (FAILURE);
	set_ip4(data, ip, get_cidr(cidr));
	set_node(&node, &data);
	listadd_head(&ip_list, node);

	return (SUCCESS);
}

#ifdef TESTING
int main(void) {
	int error;
	t_node *ip_list;
	char input[20];

	printf("debugging started > options:\n"
			"help < displays options\n"
			"<i.p.ad.dr> < test ip\n"
			"display < displays IP list\n"
			"quit/exit/ctrl+c < exits program");
	while (1) {
		fgets(input, 20, stdin);
		if ((error = parse_ip(ip_list, input)) < 0) {
			printf("parsing failed with %d\n", error);
		}
		break;
	}
	return (1);
}
#endif