#include "../../incl/hermes.h"

int				parse_port(uint16_t *port, char *input)
{
	int			ret;

	if (!input)
		return (FAILURE);
	if ((ret = atoi(input)) <= 0 || ret > PORT_MAX)
		return (FAILURE);
	*port = (uint16_t)ret;
	return (SUCCESS);
}

static int		add_port(t_portset *set, char *input)
{
	t_port		*data;
	uint16_t	port;

	if (parse_port(&port, input) == FAILURE)
		return (hermes_error(FAILURE, "bad ports specified %s", input));
	data = new_port();
	data->port = (uint16_t)port;
	if (add_node_bst(&set->ports, (void **) &data, port_cmp) == SUCCESS)
	{
		set->port_cnt++;
		return (SUCCESS);
	}
	return (FAILURE);
}

static int		add_range(t_portset *set, char **range)
{
	uint16_t	start;
	uint16_t	end;
	t_prtrng	*data;

	if (parse_port(&start, range[0]) == FAILURE)
		return (hermes_error(FAILURE, "bad start to ports range %s", range[0]));
	if (parse_port(&end, range[1]) == FAILURE)
		return (hermes_error(FAILURE, "bad end to ports range %s", range[1]));
	data = new_portrange();
	if (start > end)
		swap_uint16(&start, &end);
	data->start = start;
	data->end = end;
	data->size = end - start;
	if (add_node_bst(&set->prtrngs, (void **) &data, portrng_cmp) == true)
	{
		set->rng_cnt++;
		return (SUCCESS);
	}
	return (FAILURE);
}

int				handle_port(t_portset *set, char *input)
{
	char		*port;
	char		**port_range;

	while ((port = strsep(&input, ",")) != NULL)
	{
		if (strchr(port, '-'))
		{
			if (!(port_range = strsplit(port, '-')))
				return (hermes_error(FAILURE, "strsplit()"));
			if (add_range(set, port_range) == FAILURE)
				return (FAILURE);
			tbldel(&port_range);
		}
		else
			if (add_port(set, port) == FAILURE)
				return (FAILURE);
	}
	return (SUCCESS);
}
