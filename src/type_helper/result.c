# include "../../incl/hermes.h"

t_result		*new_result(void)
{
	t_result	*res;

	if (!(res = memalloc(sizeof(t_result))))
	{
		hermes_error(FAILURE, "malloc() %s", strerror(errno));
		return (NULL);
	}
	return (res);
}
