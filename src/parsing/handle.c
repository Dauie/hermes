# include "../../incl/job.h"
# include "../../incl/parser.h"

int			dtab_handle(t_job *job, char *opt, t_dtab *tab)
{
	int		i;
	size_t	len;
	char	*result;

	i = -1;
	len = strlen(opt);
    result = NULL;
	/* iterate TAB and look for the correct entry */
	while (result == NULL && tab[++i].name)
		result = strnstr(opt, tab[i].name, len);
	if (result)
	{
		tab[i].function(job);
		return (SUCCESS);
	}
	return (FAILURE);
}

int			dtab_wopt_handle(t_job *job, char *arg, char *opt, t_dtab_wopt *tab)
{
	int		i;
	size_t	len;
	char	*result;

	i = -1;
	len = strlen(opt);
	result = NULL;
	/* iterate TAB and look for the correct entry */
	while (result == NULL && tab[++i].name)
		result = strnstr(opt, tab[i].name, len);
	if (result)
	{
		tab[i].function(job, arg);
		return (SUCCESS);
	}
	return (FAILURE);
}
