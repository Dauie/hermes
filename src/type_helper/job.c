# include "../../incl/hermes.h"

t_job		*new_job(void)
{
	t_job	*job;

	if (!(job = (t_job*)memalloc(sizeof(t_job))))
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