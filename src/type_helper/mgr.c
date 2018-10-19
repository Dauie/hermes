# include "../../incl/hermes.h"

t_mgr		*new_mgr(void)
{
	t_mgr	*mgr;

	if (!(mgr = (t_mgr *)memalloc(sizeof(t_mgr))))
	{
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
		return (NULL);
	}
	if (!(mgr->job.opts = new_opts()))
	{
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
		return (NULL);
	}
	return (mgr);
}

