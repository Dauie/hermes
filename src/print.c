#include "../incl/hermes.h"

void	print_iprng_struct(t_node *iprng)
{
	t_ip4rng *ipr;
	t_ip4		start;
	t_ip4		end;

	ipr = (t_ip4rng*)iprng->data;
	if (!ipr)
	{
		printf("print_iprng_struct(NULL)\n");
		return ;
	}
	start.s_addr = ipr->start;
	end.s_addr = ipr->end;
	printf("ip range from: %s to %s - size: %d\n", inet_ntoa(start), inet_ntoa(end), ipr->size);
}

void	print_targetset(t_targetset *set)
{
	t_node	*seek;
	t_ip4	*ip;

	printf("\ntargetset: total:%d, ip_cnt:%d, rng_cnt:%d", set->total, set->ip_cnt, set->rng_cnt);
	seek = set->ips;
	while (seek)
	{
		ip = seek->data;
		printf("%s", inet_ntoa(*ip));
		seek = seek->right;
	};
	printf("\n");
	seek = set->iprngs;
	while (seek)
	{
		print_iprng_struct(seek);
		seek = seek->right;
	}
}