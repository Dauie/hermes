#include "../incl/hermes.h"

void	pprint_ip(uint32_t ip)
{
	struct in_addr addr;

	addr.s_addr = ip;
	printf("%s", inet_ntoa(addr));
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
	printf(" to: ");
	pprint_ip(ipr->end);
	printf(" of size: %u", ipr->size);
}

