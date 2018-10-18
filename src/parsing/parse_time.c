#include "../../incl/hermes.h"
/*
** TODO:  Eventually we want to be able to parse ms, s, m, h, and d, but
** TODO:   for now, everything should be entered in ms
*/
int			parse_time(uint32_t *opt_time, char *input)
{
	long	time;

	if (!input)
		hermes_error(FAILURE, "time not specified for parse_time()");
	if ((time = atoi(input)) < 0)
		return (hermes_error(FAILURE, "bad time specified"));
	*opt_time = (uint32_t)time;
	return (SUCCESS);
}
