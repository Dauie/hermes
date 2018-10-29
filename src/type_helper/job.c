# include "../../incl/hermes.h"

t_env		*new_job(void)
{
	t_env	*job;

	if (!(job = (t_env*)memalloc(sizeof(t_env))))
	{
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
		return (NULL);
	}
	return (job);
}

t_opts		*new_opts(void)
{
	t_opts	*opts;

	if (!(opts = memalloc(sizeof(t_opts))))
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
	return (opts);
}