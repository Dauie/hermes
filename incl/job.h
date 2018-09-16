#ifndef HERMESE_JOB_H
# define HERMESE_JOB_H

#include <stdint.h>

typedef struct			s_node
{
	struct s_node		*next;
	struct s_node		*prev;
	void				*data;
}						t_node;

typedef struct			s_ip4range
{
	uint32_t			range_size;
	uint32_t			start;
	uint32_t			end;
}						t_ip4range;

typedef struct			s_ip4
{
	uint32_t			addr;
}						t_ip4;

typedef struct			s_port
{
	uint16_t			port;
}						t_port;

typedef struct			s_portrange
{
	uint16_t			start;
	uint16_t 			end;
}						t_portrange;

typedef struct			s_targetlist
{
	size_t				ip_count;
	t_node				*ip;					/*t_node containing t_ip4 structs*/
	size_t				iprange_count;
	t_node				iprange;				/*t_node containing t_ip4range structs*/
}						t_targetlist;

typedef struct			s_portlist
{
	uint16_t			port_count;
	t_node				*ports;
	uint16_t			range_count;
	t_node				*port_range;
}						t_portlist;

typedef struct			s_port_ops
{
	uint8_t				scan_ports: 1;
	uint8_t				exclude_ports: 1;
	uint8_t				no_random: 1;
}						t_port_ops;

typedef struct			s_disc_ops
{
	uint16_t			skip_discovery: 1;		/* Skip host discovery */
	uint16_t			list_scan: 1;			/* Do not send any probes, just do reverse dns resolution on target list */
	uint16_t			ping_scan: 1;			/* Do not send any probes, just do ping scan, and traceroute or OS Detection if specified, then quit */
	uint16_t			echo: 1;				/* Send ICMP ECHO_REQUEST probe during host discovery */
	uint16_t			timestamp: 1;			/* Send ICMP_TSTAMP probe during host discovery */
	uint16_t			netmask: 1;				/* Send ICMP_MASKREQ probe during host discovery */
	uint16_t			syn: 1;					/* Send TCP SYN probe during host discovery */
	uint16_t			ack: 1;					/* Send TCP ACK probe during host discovery */
	uint16_t			udp: 1;					/* Send UDP Datagram during host discovery */
	uint16_t			tracert: 1;				/* Perform a traceroute on hosts */
}						t_disc_ops;

typedef struct			s_time_ops
{
	struct timeval		min_rtt_timeo;
	struct timeval		init_rtt_timeo;
	struct timeval		max_rtt_timeo;
	struct timeval		host_timeo;
	struct timeval		scan_delay;
	struct timeval		max_scan_delay;
}						t_time_ops;

typedef struct			s_rate_ops
{
	float				min_packet_rate;
	float				max_packet_rate;
	uint32_t			max_retries;
	uint32_t			min_parallelism;
	uint32_t			max_parallelism;
	uint32_t			min_hostgroup;
	uint32_t			max_hostgroup;
//	uint32_t			icmp_ratelimit;
//	uint32_t			rst_ratelimit;
}						t_rate_ops;

typedef struct			s_scan_ops
{
	uint8_t				syn: 1;
	uint8_t				null: 1;
	uint8_t				ack: 1;
	uint8_t				fin: 1;
	uint8_t				xmas: 1;
	uint8_t				udp: 1;
}						t_scan_ops;

typedef struct			s_osvd_ops
{
	uint8_t				svd: 1;
	uint8_t 			osd: 1;
};

typedef struct			s_out_ops
{
//	uint8_t				xml: 1;
	uint8_t				norm: 1;
	uint8_t				only_open: 1;
	uint8_t				apnd_file: 1;
//	uint8_t				iflist: 1;
	uint8_t				verbose_level; /* Default 0, verbose 1, very verbose 2 TODO: possibe enum */
}						t_out_ops;

typedef struct			s_evasion_ops
{
	uint16_t			fragment: 1;
	uint16_t			spoof_srcport: 1;
	uint16_t			spoof_srcaddr: 1;
	uint16_t			bad_checksum: 1;
	uint16_t			hex_payload: 1;
	uint16_t			ascii_payload: 1;
	uint16_t			rand_payload: 1;
	uint16_t			ip_ttl;
	uint16_t			rpayload_len;
	uint16_t			mtu;
	uint16_t			spoofed_srcport;
	uint32_t			spoofed_srcaddr;
}						t_evasion_ops;

typedef struct			s_ops
{
	uint8_t				thread_count;
	t_port_ops			portops;
	t_scan_ops			scanops;
//	t_osvd_ops			osvdops;
	t_disc_ops			discops;
	t_time_ops			timeops;
	t_rate_ops			rateops;
	t_evasion_ops		evasops;
	t_out_ops			outops;
}						t_ops;

typedef struct			s_worker
{
	t_ip4 				ip;
	t_port				port;
}						t_worker;

typedef struct			s_job
{
	t_ops				options;
	t_targetlist		*targets;
	t_targetlist		*exclude_targets;
	t_node				*worker_list; /* node pop. w/ worker type -> next worker */
	t_portlist			*scan_portlist;
	t_portlist			*exclude_portlist;
	t_portlist			*d_syn_portlist;
	t_portlist			*d_ack_portlist;
	t_portlist			*d_udp_portlist;
	void				*custom_payload;
	FILE				*resume_file;
	FILE				*xml_file;
	FILE				*norm_file;
}						t_job;

#endif
