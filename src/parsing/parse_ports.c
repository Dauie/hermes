#include "../../incl/hermes.h"
#include "../../incl/parser.h"

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

static int		add_port(t_portlist *list, char *input)
{
	t_node		*node;
	t_port		*data;
	uint16_t	port;
	/*
	** check to make sure port is in range.
	*/
	if (parse_port(&port, input) == FAILURE)
		return (hermes_error(FAILURE, FALSE, 1, "bad port specified", input));
	data = new_port();
	node = new_node();
	data->port = (uint16_t)port;
	node->data = data;
	if (add_node(&list->ports, &node, port_cmp) == SUCCESS)
	{
		list->port_count++;
		return (SUCCESS);
	}
	return (FAILURE);
}

static int		add_range(t_portlist *list, char **range)
{
	uint16_t	start;
	uint16_t	end;
	t_node		*node;
	t_portrange	*data;

	if (parse_port(&start, range[0]) == FAILURE)
		return (hermes_error(FAILURE, FALSE, 1, "bad start to port range", range[0]));
	if (parse_port(&end, range[1]) == FAILURE)
		return (hermes_error(FAILURE, FALSE, 1, "bad end to port range", range[1]));
	node = new_node();
	data = new_portrange();
	data->start = start;
	data->end = end;
	node->data = data;
	if (add_node(&list->port_range, &node, portrange_cmp) == SUCCESS)
	{
		list->range_count++;
		return (SUCCESS);
	}
	return (FAILURE);
}

int				handle_port(t_portlist *list, char *input)
{
	char		*port;
	char		**port_range;

	/* TODO: Make sure all portlists in job are free'd */
	while ((port = strsep(&input, ",")) != NULL)
	{
		if (strchr(port, '-'))
		{
			if (!(port_range = strsplit(port, '-')))
				return (hermes_error(INPUT_ERROR, TRUE, 1, "strsplit()"));
			if (add_range(list, port_range) == FAILURE)
				return (FAILURE);
			tbldel(&port_range);
		}
		else
			if (add_port(list, port) == FAILURE)
				return (FAILURE);
	}
	return (SUCCESS);
}
