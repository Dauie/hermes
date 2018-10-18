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

static int		add_port(t_portset *list, char *input)
{
	t_port		*data;
	uint16_t	port;

	if (parse_port(&port, input) == FAILURE)
		return (hermes_error(FAILURE, 1, "bad ports specified", input));
	data = new_port();
	data->port = (uint16_t)port;
	if (add_node_bst(&list->ports, (void **) &data, port_cmp) == SUCCESS)
	{
		list->port_cnt++;
		return (SUCCESS);
	}
	return (FAILURE);
}

static int		add_range(t_portset *list, char **range)
{
	uint16_t	start;
	uint16_t	end;
	t_prtrng	*data;

	if (parse_port(&start, range[0]) == FAILURE)
		return (hermes_error(FAILURE, 1, "bad start to ports range", range[0]));
	if (parse_port(&end, range[1]) == FAILURE)
		return (hermes_error(FAILURE, 1, "bad end to ports range", range[1]));
	data = new_portrange();
	if (start > end)
		swap_uint16(&start, &end);
	data->start = start;
	data->end = end;
	data->size = end - start;
	if (add_node_bst(&list->prtrngs, (void **) &data, portrng_cmp) == true)
	{
		list->rng_cnt++;
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
				return (hermes_error(FAILURE, 1, "strsplit()"));
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
