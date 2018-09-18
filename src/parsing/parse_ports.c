#include "../../incl/hermes.h"
#include "../../incl/parser.h"


static int		add_port(t_portlist *list, char *prt)
{
	t_node		*node;
	t_port		*data;
	int			port;
	/*
	** check to make sure port is in range.
	*/
	port = atoi(prt);
	if (port > PORT_MAX || port <= 0)
		return (hermes_error(INPUT_ERROR, TRUE, 1, "port specified is not in range"));

	node = new_node();
	if (!(data = (t_port *)memalloc(sizeof(t_port))))
		return (hermes_error(INPUT_ERROR, TRUE, 2, "malloc()", strerror(errno)));
	data->port = (uint16_t)port;
	node->data = data;
	listadd_head(&list->ports, node);
	list->port_count++;
	return (SUCCESS);
}

static int		add_range(t_portlist *list, char **range)
{
	uint16_t	start;
	uint16_t	end;
	t_node		*node;
	t_portrange	*data;

	if (parse_port(&start, range[0]) == FAILURE)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad start to port range", range[0]);
	if (parse_port(&end, range[1]) == FAILURE)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad end to port range", range[1]);
	node = new_node();
	if (!(data = (t_portrange *)memalloc(sizeof(t_portrange))))
		return (hermes_error(errno, TRUE, 2, "malloc()", strerror(errno)));
	data->start = start;
	data->end = end;
	node->data = data;
	listadd_head(&list->port_range, node);
	list->range_count++;
	return (SUCCESS);
}

int				parse_port(uint16_t *port, char *port_str)
{
	int			ret;

	if ((ret = atoi(port_str)) <= 0 || ret > PORT_MAX)
		return (FAILURE);
	*port = (uint16_t)ret;
	return (SUCCESS);
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
