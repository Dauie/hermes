#ifndef NMAP_H
# define NMAP_H

# include <stdlib.h>
# include <pthread.h>
# include <netdb.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <netinet/udp.h>
# include <netinet/tcp.h>

typedef struct		s_ip4range
{
	u_int32_t		start;
	u_int32_t		end;
}					t_ip4range;

typedef struct			s_ip4_node
{
	u_int32_t			addr;
	struct s_ip4_node	*next;
}						t_ip4node;

typedef struct				s_ip4range_node
{
	t_ip4range				range;
	struct s_ip4range_node	*next;
}							s_ip4range_node;

/*
** If allocated the port list's first member will be the array's length
*/
typedef struct			s_disc_portlists
{
	uint16_t			*syn_portlist;
	uint16_t			*ack_portlist;
	uint16_t			*udp_portlist;
}						t_disc_portlists;

typedef struct			s_port_ops
{
	uint8_t				ports_specified: 1;
	uint8_t				exclude_specified: 1;
	uint8_t				do_less: 1;
	uint8_t				do_common: 1;
	uint8_t				no_random: 1;
}						t_port_ops;

typedef struct			s_disc_ops
{
	uint16_t			skip: 1;
	uint16_t			ping_scan: 1;
	uint16_t			list_scan: 1;
	uint16_t			syn: 1;
	uint16_t			ack: 1;
	uint16_t			udp: 1;
	uint16_t			netmask: 1;
	uint16_t			timestamp: 1;
	uint16_t			echo: 1;
	uint16_t			tracert: 1;
}						t_disc_ops;


typedef struct			s_ops
{
	t_disc_ops			discovery;
	t_port_ops			ports;

}						t_ops;
typedef struct			s_scanjob
{
	t_ops				options;
	t_ip4node			*targets;
	t_ip4range			*target_ranges;
	t_disc_portlists	*disc_portlists;

}						t_scanjob;

#endif
