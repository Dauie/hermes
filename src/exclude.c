#include "../incl/job.h"
#include "../incl/hermes.h"
#include "../incl/parser.h"

void	exclude_ip4(t_targetlist *list, t_node **targets, t_node *exclude)
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

void	split_range(t_ip4range *target, t_ip4range  *exclude,
					t_ip4range **l, t_ip4range **r)
{
	*l = NULL;
	*r = NULL;

	if (ip4_cmp(&exclude->start, &target->start) > 0 &&
			ip4_cmp(&exclude->end, &target->end) < 0)
	{
		ip4_add(&exclude->start, 1);
		*l = new_ip4range();
		*r = new_ip4range();
		(*r)->start = exclude->end + 1;
		(*r)->end = target->end;
		(*r)->range_size = (*r)->start - (*r)->end;
		(*l)->start = target->start;
		(*l)->end = exclude->start - 1;
		(*l)->range_size = (*l)->start - (*l)->end;
	}
	else if (ip4_cmp(&exclude->end, &target->start) >= 0 &&
			ip4_cmp(&exclude->end, &target->end) < 0)
	{
		*r = new_ip4range();
		(*r)->start = exclude->end + 1;
		(*r)->end = target->end;
		(*r)->range_size = (*r)->start - (*r)->end;
	}
	else if (ip4_cmp(&exclude->start, &target->end) <= 0 &&
			 ip4_cmp(&exclude->end, &target->end) > 0)
	{
		*l = new_ip4range();
		(*l)->start = target->start;
		(*l)->end = exclude->start - 1;
		(*l)->range_size = (*l)->start - (*l)->end;
	}
}

void		exclude_ip4range(t_targetlist *list, t_node **targets, t_node *exclude)
{
	t_node	*conflict;
	t_ip4range *range;
	t_ip4range *left;
	t_ip4range *right;

	if (!exclude)
		return ;
	if (exclude->left)
		exclude_ip4range(list, targets, exclude->left);
	if ((conflict = tree_search(targets, exclude->data, ip4rng_overlap_cmp)))
	{
		range = new_ip4range();
		memcpy(range, conflict->data, sizeof(t_ip4range));
		remove_node(targets, range, ip4rng_cmp, ip4rng_min);
		split_range(range, exclude->data, &left, &right);
		printf("%d\n", ((t_ip4range*)(conflict)->data)->range_size);
	}
	if (exclude->right)
		exclude_ip4range(list, targets, exclude->right);
}

int		do_exclusions( t_targetlist *targets, t_targetlist *exclude)
{
	if (!targets || !exclude)
		return (0);
	if (targets->ip && exclude->ip)
		exclude_ip4(targets, &targets->ip, exclude->ip);
	if (targets->iprange && exclude->iprange)
		exclude_ip4range(targets, &targets->iprange, exclude->iprange);
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


