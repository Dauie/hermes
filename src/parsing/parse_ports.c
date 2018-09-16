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
		return (FAILURE);															/* TODO: add hermese_error call*/
	if (!(node = (t_node *)memalloc(sizeof(t_node))))
		return (FAILURE);															/* TODO: add hermese_error call*/
	if (!(data = (t_port *)memalloc(sizeof(t_port))))
		return (FAILURE);															/* TODO: add hermese_error call*/
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
		return (FAILURE);															/* TODO: add hermese_error call*/
	if (!(node = (t_node *)memalloc(sizeof(t_node))))
		return (FAILURE);															/* TODO: add hermese_error call*/
	if (!(data = (t_portrange *)memalloc(sizeof(t_portrange))))
		return (FAILURE);															/* TODO: add hermese_error call*/
	data->start = (uint16_t)start;
	data->end = (uint16_t)end;
	node->data = data;
	listadd_head(&list->port_range, node);
	list->range_count++;
	return (SUCCESS);
}

int				parse_port(t_portlist **list, char *input)
{
	char		*port;
	char		**port_range;

																				/* TODO: Make sure all portlists in job are free'd */
	if (!(*list = memalloc(sizeof(t_portlist))))
		return (FAILURE);
	while ((port = strsep(&input, ",")) != NULL)
	{
		if (strchr(port, '-'))
		{
			if (!(port_range = strsplit(port, '-')))							/* TODO: make strsplit implementation for libhermese */
				return (FAILURE);													/* TODO: add hermese_error() call */
			if (add_range(*list, port_range) == FAILURE)
				return (FAILURE);
			tbldel(&port_range);												/* TODO: add tbldel to libhermese */
		} else
			if (add_port(*list, port) == FAILURE)
				return (FAILURE);
	}
	return (SUCCESS);
}
