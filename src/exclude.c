#include "../incl/hermes.h"
#include "../incl/job.h"

int    exclude_ips(t_node **in, t_node **ex)
{
	t_node  *head_in;
	t_node  *head_ex;

	head_in = *in;
	while ((*in)->data)
	{
		head_ex = *ex;
		while ((*ex)->data)
		{
			if ((t_ip4*(*in)->data)->addr == (t_ip4*(*ex)->data)->addr)
				list_remove(in);
			*ex = (*ex)->next;
		}
		*in = (*in)->next;
		*ex = head_ex;
	}
	*in = head_in;
	return (0);
}

int    exclude_ipranges(t_node **include, t_node **exclude)
{
	t_ip4range	*dat_in;
	t_ip4range	*dat_ex;
	t_node		*head_in;
	t_node		*head_ex;

	head_in = *include;
	while ((*include)->data)
	{
		head_ex = *exclude;
		while ((*exclude)->data)
		{
			dat_in = (*include)->data;
			dat_ex = (*exclude)->data;
			if (!(dat_in->start > dat_ex->end || dat_in->end < dat_ex->start))
			{
				if (dat_in->start <= dat_ex->start)
				{
					if (dat_in->end <= dat_ex->end)
						split_range(dat_ex->start - dat_in->start);
					else if (dat_in->end >= dat_ex->end)
						split_range(dat_in->end - dat_ex->end);
				}
				else if (dat_in->start >= dat_ex->start)
				{
					if (dat_in->end >= dat_ex->end)
						split_range(dat_in->end - dat_ex->end);
				}
				list_remove(include);
			}
			*exclude = (*exclude)->next;
		}
		*include = (*include)->next;
		*exclude = head_ex;
	}
	*include = head_in;
	return (0);
}

int        do_exclusions(t_targetlist **in, t_targetlist **ex)
{
	if (!*in || !*ex)
		return (0);
	if ((*in)->ip && (*ex)->ip)
		if (exclude_ips(&(*in)->ip, &(*ex)->ip) < 0)
            return (hermes_error(INPUT_ERROR, FALSE, 1, "IP exclusion"));
	if ((*in)->iprange && (*ex)->iprange)
		if (exclude_ipranges(&(*in)->iprange, &(*ex)->iprange) < 0)
			return (hermes_error(INPUT_ERROR, FALSE, 1, "IP exclusion"));
	return (0);
}
