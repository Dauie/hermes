#include "../../incl/parser.h"
#include "../../incl/defined.h"

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

	if (!(node = (t_node *)memalloc(sizeof(t_node))))
		return (hermes_error(INPUT_ERROR, TRUE, 2, "malloc()", strerror(errno)));

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
	int			start;
	int			end;
	t_node		*node;
	t_portrange	*data;

	start = atoi(range[0]);
	end = atoi(range[1]);
	/*
	** check to make sure port is in range.
	*/
	if (start > PORT_MAX || end > PORT_MAX || start <= 0 || end <= 0)
		return (hermes_error(INPUT_ERROR, TRUE, 1, "port specified is not in range"));

	if (!(node = (t_node *)memalloc(sizeof(t_node))))
		return (hermes_error(INPUT_ERROR, TRUE, 2, "malloc()", strerror(errno)));

	if (!(data = (t_portrange *)memalloc(sizeof(t_portrange))))
		return (hermes_error(INPUT_ERROR, TRUE, 2, "malloc()", strerror(errno)));

	data->start = (uint16_t)start;
	data->end = (uint16_t)end;
	node->data = data;
	listadd_head(&list->port_range, node);
	list->range_count++;
	return (SUCCESS);
}

int				get_port(char *port_str)
{
	int			port;

	if ((port = atoi(port_str)) <= 0 || port > PORT_MAX)
		return (-1); /*hermes error fatal*/
	return (port);
}

int				parse_port(t_portlist **list, char *input)
{
	char		*port;
	char		**port_range;

	/* TODO: Make sure all portlists in job are free'd */
	if (!(*list = memalloc(sizeof(t_portlist))))
		return (hermes_error(INPUT_ERROR, TRUE, 2, "malloc()", strerror(errno)));
	while ((port = strsep(&input, ",")) != NULL)
	{
		if (strchr(port, '-'))
		{
			if (!(port_range = strsplit(port, '-')))
				return (hermes_error(INPUT_ERROR, TRUE, 1, "strsplit() failed"));
			if (add_range(*list, port_range) == FAILURE)
				return (FAILURE);
			tbldel(&port_range);
		}
		else
			if (add_port(*list, port) == FAILURE)
				return (FAILURE);
	}
	return (SUCCESS);
}
