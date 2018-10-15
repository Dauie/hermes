//
// Created by Samuel Collet on 10/12/18.
//
# include "../../incl/hermes.h"
# include "../../incl/parser.h"
# include <assert.h>

int main(void)
{
//	uint32_t 	size;
//	char 		ip1_s[20];
//	char		ip2_s[20];
//	uint32_t	ip1_n = htonl(169345025); //192.168.12.10
//	uint32_t	ip2_n = htonl(3237937164); //192.255.0.12
//	uint32_t	ip1_h = 169345025;
//	uint32_t	ip2_h = 3237937164;
//
//
//	inet_ntop(AF_INET, &ip1_n, ip1_s, 20);
//	inet_ntop(AF_INET, &ip2_n, ip2_s, 20);
//	printf("ipaddrs\n%s\n%s\n", ip1_s, ip2_s);
//	size = (ntohl(ip2_n) - ntohl(ip1_n)) / 4;
//	printf("size\n%d\n\n", size);
//	while (ip1_h < ip2_h)
//	{
//		ip1_n = htonl(ip1_h); //192.168.12.10
//		ip2_n = htonl(ip2_h); //192.255.0.12
//		inet_ntop(AF_INET, &ip1_n, ip1_s, 20);
//		inet_ntop(AF_INET, &ip2_n, ip2_s, 20);
//		printf("ipaddrs\n%s\n%s\n", ip1_s, ip2_s);
//		ip1_h += size;
//	}
	struct in_addr r[8];
	int i = 0;
	t_ip4rng	range;
	in_addr_t start;
	in_addr_t mask;
	char *ip = "192.168.1.0";
	char *cidr = "24";

	parse_ip(&r[0].s_addr,"192.168.1.0");
	parse_ip(&r[1].s_addr,"192.168.1.63");

	parse_ip(&r[2].s_addr,"192.168.1.64");
	parse_ip(&r[3].s_addr,"192.168.1.127");

	parse_ip(&r[4].s_addr,"192.168.1.128");
	parse_ip(&r[5].s_addr,"192.168.1.191");

	parse_ip(&r[6].s_addr,"192.168.1.192");
	parse_ip(&r[7].s_addr,"192.168.1.255");

	t_node *tree;
	parse_ip(&start, ip);
	parse_cidr_mask(&mask, cidr);
	set_ip4range(&range, &start, &mask);
	tree = split_ip4rng_n(&range, 4);
	while (i < 8 && tree) {
		assert(((t_ip4rng*)tree->data)->start == r[i++].s_addr);
		assert(((t_ip4rng*)tree->data)->end == r[i++].s_addr);
		tree = tree->right;
	}
	return (0);
}