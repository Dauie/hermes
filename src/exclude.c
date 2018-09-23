#include "../incl/job.h"
#include "../incl/hermes.h"
#include "../incl/parser.h"


//void	split_range(t_node **include, uint32_t start, uint32_t end)
//{
//	*include = new_node();
//	(*include)->data = new_ip4range();
//	((t_ip4range*)(*include)->data)->start = end - start;
//	((t_ip4range*)(*include)->data)->start = start;
//	((t_ip4range*)(*include)->data)->start = end;
//
//}

void exclude_ip4range(t_targetlist *list, t_node **targets, t_node *exclude)
{
	if (!exclude)
		return ;
	if (exclude->left)
		exclude_ip4range(list, targets, exclude->left);
	if (remove_node(targets, exclude->data, ip4range_cmp, ip4range_min) == SUCCESS)
	{
		list->iprange_count--;
		list->total--;
	}
	if (exclude->right)
		exclude_ip4range(list, targets, exclude->right);
}

void 	exclude_ip4(t_targetlist *list, t_node **targets, t_node *exclude)
{
	if (!exclude)
		return ;
	if (exclude->left)
		exclude_ip4(list, targets, exclude->left);
	if (remove_node(targets, exclude->data, ip4_cmp, ip4_min) == SUCCESS)
	{
		list->ip_count--;
		list->total--;
	}
	if (exclude->right)
		exclude_ip4(list, targets, exclude->right);
}

int		do_exclusions( t_targetlist *targets, t_targetlist *exclude)
{
	if (!targets || !exclude)
		return (0);
	if (targets->ip && exclude->ip)
		exclude_ip4(targets, &targets->ip, exclude->ip);
//	if ((*targets)->iprange && (*exclude)->iprange)
//		if (ex_ipranges(&(*targets)->iprange, &(*exclude)->iprange) < 0)
//			return (hermes_error(INPUT_ERROR, FALSE, 1, "IP exclusion"));
	return (0);
}

//int		ex_ipranges(t_node **in, t_node **ex)
//{
//	t_node		*tmp;
//	t_ip4range	*dat_in;
//	t_ip4range	*dat_ex;
//	t_node		*head_in;
//	t_node		*head_ex;
//
//	head_in = *in;
//	while ((*in)->data)
//	{
//		head_ex = *ex;
//		while ((*ex)->data)
//		{
//			dat_in = (*in)->data;
//			dat_ex = (*ex)->data;
//
//			*ex = (*ex)->next;
//		}
//		*in = (*in)->next;
//		*ex = head_ex;
//	}
//	*in = head_in;
//	return (0);
//}

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

