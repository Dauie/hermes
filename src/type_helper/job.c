# include "../../incl/hermes.h"

t_job		*new_job(void)
{
	t_job	*job;

	if (!(job = (t_job*)memalloc(sizeof(t_job))))
		hermes_error(FAILURE, 2, "malloc()", strerror(errno));
	return (job);
}