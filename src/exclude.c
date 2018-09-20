//#include "../incl/job.h"
//#include "../incl/hermes.h"
//#include "../incl/parser.h"
//
//
//void	split_range(t_node **include, uint32_t start, uint32_t end)
//{
//	*include = new_node();
//	(*include)->data = new_ip4range();
//	((t_ip4range*)(*include)->data)->start = end - start;
//	((t_ip4range*)(*include)->data)->start = start;
//	((t_ip4range*)(*include)->data)->start = end;
//
//}
//
//int		ex_ips(t_node **in, t_node **ex)
//{
//	t_node  *head_in;
//	t_node  *head_ex;
//
//	head_in = *in;
//	while ((*in)->data)
//	{
//		head_ex = *ex;
//		while ((*ex)->data)
//		{
//			if ((t_ip4*(*in)->data)->addr == (t_ip4*(*ex)->data)->addr)
//				list_remove(in);
//			*ex = (*ex)->next;
//		}
//		*in = (*in)->next;
//		*ex = head_ex;
//	}
//	*in = head_in;
//	return (0);
//}
//
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
//
//int		do_exclusions(t_targetlist **include, t_targetlist **exclude)
//{
//	if (!*include || !*exclude)
//		return (0);
//	if ((*include)->ip && (*exclude)->ip)
//		if (ex_ips(&(*include)->ip, &(*exclude)->ip) < 0)
//            return (hermes_error(INPUT_ERROR, FALSE, 1, "IP exclusion"));
//	if ((*include)->iprange && (*exclude)->iprange)
//		if (ex_ipranges(&(*include)->iprange, &(*exclude)->iprange) < 0)
//			return (hermes_error(INPUT_ERROR, FALSE, 1, "IP exclusion"));
//	return (0);
//}
