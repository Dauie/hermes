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

typedef struct			s_time_ops
{
	struct timeval		min_rtt_timeo;
	struct timeval		max_rtt_timeo;
	struct timeval		host_timeo;
	struct timeval		scan_delay;
	struct timeval		max_scan_delay;
}						t_time_ops;

typedef struct			s_rate_ops
{
	uint				min_rate;
	uint 				max_rate;
	uint				max_retries;
}						t_rate_ops;

typedef struct			s_scan_ops
{
	uint8_t				syn: 1;
	uint8_t				null: 1;
	uint8_t				ack: 1;
	uint8_t				fin: 1;
	uint8_t				xmas: 1;
	uint8_t				udp: 1;
	uint8_t				svd: 1;
	uint8_t 			osd: 1;
}						t_scan_ops;

typedef struct			s_out_ops
{
	uint8_t				verbose: 1;
	uint8_t				vverbose: 1;
	uint8_t				xml: 1;
	uint8_t				norm: 1;
	uint8_t				only_open: 1;
	uint8_t				apnd_file: 1;
	uint8_t				iflist: 1;
}						t_out_ops;

typedef struct			s_evasion_ops
{
	uint8_t				fragment: 1;
	uint8_t				spoof_port: 1;
	uint8_t				spoof_addr: 1;
	uint8_t				decoys_specified: 1;
	uint8_t				bogus_checksum: 1;
	uint8_t				hex_payload: 1;
	uint8_t				ascii_payload: 1;
	uint8_t				rand_payload: 1;
	uint16_t			rpayload_len;
	uint16_t			mtu;
	uint16_t			port;
	uint32_t			addr;
}						t_evasion_ops;

typedef struct			s_ops
{
	uint8_t				thread_count;
	t_port_ops			portops;
	t_scan_ops			scanops;
	t_disc_ops			discops;
	t_time_ops			timeops;
	t_rate_ops			rateops;
	t_evasion_ops		evasion;
}						t_ops;

typedef struct			s_scanjob
{
	t_ops				options;
	u_int16_t			*ports;
	t_ip4node			*targets;
	t_ip4range			*target_ranges;
	t_ip4node			*decoys;
	t_disc_portlists	*disc_portlists;
	void				*custom_payload;
}						t_scanjob;

#endif
