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
//			if (!(dat_in->start > dat_ex->end || dat_in->end < dat_ex->start))
//			{
//				tmp = *in;
//				list_remove(in);
//				if (dat_in->start <= dat_ex->start)
//				{
//					if (dat_in->end <= dat_ex->end)
//						split_range(&tmp, dat_in->start, dat_ex->start);
//					else if (dat_in->end >= dat_ex->end)
//					{
//						split_range(&tmp, dat_ex->end, dat_in->end);
//						split_range(&tmp, dat_in->start, dat_ex->start)
//					}
//				}
//				else if (dat_in->start >= dat_ex->start)
//				{
//					if (dat_in->end >= dat_ex->end)
//						split_range(in, dat_ex->end, dat_in->end);
//				}
//			}
//			*ex = (*ex)->next;
//		}
//		*in = (*in)->next;
//		*ex = head_ex;
//	}
//	*in = head_in;
//	return (0);
//}


