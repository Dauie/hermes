#include "../../incl/job.h"

t_targetset		*new_targetset(void)
{
	t_targetset *new;

	if (!(new = memalloc(sizeof(t_targetset))))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	return (new);
}

