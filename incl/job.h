#ifndef JOB_H
# define JOB_H

# include <stdint.h>
# include <stddef.h>
# include <netinet/in.h>
# include "../libhermes/incl/libhermes.h"

typedef struct			s_ip4rng
{
	uint32_t			size;
	in_addr_t			start;
	in_addr_t			end;
}						t_ip4rng;

typedef struct in_addr	t_ip4;

typedef struct			s_ip4bytes
{
	uint8_t				b1;
	uint8_t				b2;
	uint8_t				b3;
	uint8_t				b4;
}						t_ip4bytes;

typedef struct			s_port
{
	uint16_t			port;
}						t_port;

typedef struct			s_prtrng
{
	uint16_t			size;
	uint16_t			start;
	uint16_t 			end;
}						t_prtrng;

typedef struct			s_workerlist
{
	uint32_t			wrkr_cnt;
	t_node				*wrkrs;					/*t_node list containing t_ip4 structs*/
}						t_workerlist;

typedef struct			s_targetlist
{
	uint32_t			total;
	uint32_t			ip_cnt;
	uint32_t			rng_cnt;
	t_node				*ips;						/*t_node list containing t_ip4 structs*/
	t_node				*iprngs;					/*t_node list containing t_ip4rng structs*/
}						t_targetlist;

typedef struct			s_portlist
{
	uint16_t			total;
	uint16_t			port_cnt;
	uint16_t			rng_cnt;
	t_node				*ports;
	t_node				*prtrngs;
}						t_portlist;

typedef struct			s_optbitf
{
	uint64_t			scan_ports_spec: 1;
	uint64_t			exclude_ports_spec: 1;
	uint64_t			ports_no_random: 1;
	uint64_t			skip_hst_discov: 1;			/* Skip host discovery */
	uint64_t			do_list_scan: 1;			/* Do not send any probes, just do reverse dns resolution on target list */
	uint64_t			do_ping_scan: 1;			/* Do not send any probes, just do ping scan, and traceroute or OS Detection if specified, then quit */
	uint64_t			do_echo_discov: 1;			/* Send ICMP ECHO_REQUEST probe during host discovery */
	uint64_t			do_tstamp_discov: 1;		/* Send ICMP_TSTAMP probe during host discovery */
	uint64_t			do_nmask_discov: 1;			/* Send ICMP_MASKREQ probe during host discovery */
	uint64_t			do_syn_discov: 1;			/* Send TCP SYN probe during host discovery */
	uint64_t			do_ack_discov: 1;			/* Send TCP ACK probe during host discovery */
	uint64_t			do_udp_discov: 1;			/* Send UDP Datagram during host discovery */
	uint64_t			do_syn_scan: 1;
	uint64_t			do_null_scan: 1;
	uint64_t			do_ack_scan: 1;
	uint64_t			do_fin_scan: 1;
	uint64_t			do_xmas_scan: 1;
	uint64_t			do_udp_scan: 1;
	uint64_t			do_svd: 1;
	uint64_t 			do_osd: 1;
	uint64_t			do_iflist: 1;
	uint64_t			do_tracert: 1;				/* Perform a traceroute on hosts */
	uint64_t			fragment_pkts: 1;
	uint64_t			spoof_srcport: 1;
	uint64_t			spoof_srcaddr: 1;
	uint64_t			do_bad_checksum: 1;
	uint64_t			custom_hex_payload: 1;
	uint64_t			custom_ascii_payload: 1;
	uint64_t			custom_rand_payload: 1;
	uint64_t			custom_ip_ttl : 1;
	uint64_t			output_xml: 1;
	uint64_t			output_norm: 1;
	uint64_t			show_only_open_ports: 1;
	uint64_t			apnd_file: 1;
}						t_optbitf;

typedef struct			s_ops
{
	t_optbitf			bitops;
	uint32_t			min_rtt_timeo;
	uint32_t			init_rtt_timeo;
	uint32_t			max_rtt_timeo;
//	uint32_t			icmp_ratelimit;
//	uint32_t			rst_ratelimit;
	uint32_t			host_timeo;
	uint32_t			scan_delay;
	uint32_t			max_scan_delay;
	uint32_t			min_packet_rate;
	uint32_t			max_packet_rate;
	uint32_t			max_retries;
	uint32_t			min_parallel;
	uint32_t			max_parallel;
	uint32_t			min_hostgroup;
	uint32_t			max_hostgroup;
	uint32_t			spoofed_srcaddr;
	uint16_t			spoofed_srcport;
	uint16_t			rpayload_len;
	uint8_t				fragment_mtu;
	uint8_t				ip_ttl;
	uint8_t				thread_count;
	uint8_t				verbose_level; /* Default 0, verbose 1, very verbose 2 TODO: possibe enum */
}						t_ops;

typedef struct			s_job
{
	t_ops				options;
	t_targetlist		targets;
	t_portlist			ports;
	t_portlist			syn_ports;
	t_portlist			ack_ports;
	t_portlist			udp_ports;
	void				*custom_payload;
}						t_job;

typedef struct			s_worker
{
    struct sockaddr_in	sin;
    int 				sock;
    t_job				*job;
}						t_worker;

typedef struct			s_mgr {
	t_job				job;
	uint32_t            thread_count;
	t_targetlist		exclude_targets;
	t_portlist			exclude_ports;
	t_workerlist		worker_list;
	FILE				*resume_file;
	FILE				*xml_file;
	FILE				*norm_file;
}						t_mgr;

t_job		*new_job(void);

#endif
