#ifndef PARSER_H
# define PARSER_H

# include "job.h"
# include "defined.h"

/*
**	Dispatch Tables
*/

typedef struct	s_dtab_wopt
{
	char		*name;
	void		(*function)(t_mgr *, char *);
}				t_dtab_wopt;

typedef struct	s_dtab
{
	char		*name;
	void		(*function)(t_mgr *);
}				t_dtab;

//typedef enum	s_types
//{
//}				t_types;

int				dtab_loop(t_mgr *mgr, char *opt, t_dtab *tab);
int				dtab_wopt_loop(t_mgr *mgr, char *arg, char *opt,
								  t_dtab_wopt *tab);
int				parse_port(uint16_t *port, char *input);
int				handle_port(t_portlist *list, char *input);
int				parse_time(uint32_t *opt_time, char *input);
int				parse_opts(t_mgr *mgr, int ac, char **args);
int				handle_ip(t_targetlist *targets, char *input);
int				parse_ip(in_addr_t *ip, char *input);
int				parse_worker(t_workerlist *workerlist, char *input);
int				parse_worker_file(t_workerlist *list, char *filename);
int				parse_ip_file(t_targetlist *list, char *filename);

/*
**	boolean user opts
*/
void			h_ack_scan(t_mgr *mgr);
void			h_append_output(t_mgr *mgr);
void			h_bad_checksum(t_mgr *mgr);
void			h_dont_randomize_ports(t_mgr *mgr);
void			h_fin_scan(t_mgr *mgr);
void			h_icmp_host_discovery(t_mgr *mgr);
void			h_list_scan(t_mgr *mgr);
void			h_nmask_host_discovery(t_mgr *mgr);
void			h_norm_output(t_mgr *mgr);
void			h_null_scan(t_mgr *mgr);
void			h_ping_scan(t_mgr *mgr);
void			h_show_only_open_ports(t_mgr *mgr);
void			h_skip_host_discovery(t_mgr *mgr);
void			h_syn_scan(t_mgr *mgr);
void			h_tstamp_host_discovery(t_mgr *mgr);
void			h_traceroute(t_mgr *mgr);
void			h_udp_scan(t_mgr *mgr);
void			h_xmas_scan(t_mgr *mgr);

/*
**	user opts with specification
*/
void			h_ack_portlist(t_mgr *mgr, char *input); /*done*/
void			h_daemon(t_mgr *mgr, char *input);
void			h_custom_payload_hex(t_mgr *mgr, char *input);
void			h_custom_random_payload(t_mgr *mgr, char *input); /*done*/
void			h_custom_payload_ascii(t_mgr *mgr, char *input); /*done*/
void			h_exclude_targets(t_mgr *mgr, char *input);
void			h_exclude_target_file(t_mgr *mgr, char *input);
void			h_exclude_ports(t_mgr *mgr, char *input); /*done*/
void			h_fragment_mtu(t_mgr *mgr, char *input);
void			h_host_timeout(t_mgr *mgr, char *input);
void			h_custom_ip_ttl(t_mgr *mgr, char *input);
void			h_target_file(t_mgr *mgr, char *input);
void			h_max_hostgroup(t_mgr *mgr, char *input);
void			h_max_packet_rate(t_mgr *mgr, char *input);
void			h_max_retries(t_mgr *mgr, char *input);
void			h_max_rtt_timeout(t_mgr *mgr, char *input);
void			h_min_hostgroup(t_mgr *mgr, char *input);
void			h_min_packet_rate(t_mgr *mgr, char *input);
void			h_min_rtt_timeout(t_mgr *mgr, char *input);
void			h_scan_portlist(t_mgr *mgr, char *input);
void			h_scan_delay(t_mgr *mgr, char *input);
void			h_max_scan_delay(t_mgr *mgr, char *input);
void			h_spoof_srcip(t_mgr *mgr, char *input);
void			h_spoof_srcport(t_mgr *mgr, char *input);
void			h_syn_portlist(t_mgr *mgr, char *input);
void			h_udp_portlist(t_mgr *mgr, char *input);
void			h_thread_amt(t_mgr *mgr, char *input);
void			h_worker(t_mgr *mgr, char *input);
void			h_worker_file(t_mgr *mgr, char *input);


#endif
