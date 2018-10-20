#include "../incl/hermes.h"

void	pprint_ip(uint32_t ip)
{
	int bytes[4];

	bytes[0] = ip & 0xFF;
	bytes[1] = (ip >> 8) & 0xFF;
	bytes[2] = (ip >> 16) & 0xFF;
	bytes[3] = (ip >> 24) & 0xFF;
	printf("%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
}

void	print_ip_struct(t_node *ip4)
{
	t_ip4 *ip;

	ip = (t_ip4*)ip4->data;
	if (!ip)
	{
		printf("print_ip_struct(NULL)\n");
		return ;
	}
	printf("ip: ");
	pprint_ip(ip->s_addr);
	printf("\n");
}

void	print_iprng_struct(t_node *iprng)
{
	t_ip4rng *ipr;

	ipr = (t_ip4rng*)iprng->data;
	if (!ipr)
	{
		printf("print_iprng_struct(NULL)\n");
		return ;
	}
	printf("ip range from: ");
	pprint_ip(ipr->start);
	printf("to: ");
	pprint_ip(ipr->end);
	printf("of size: %d\n", ipr->size);
}

