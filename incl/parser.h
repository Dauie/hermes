#ifndef PARSER_H
# define PARSER_H

#ifndef HERMES_H
# include "hermes.h"
#endif

/*
**	Dispatch Tables
*/

typedef struct	s_dtab_wopt
{
	char		*name;
	int			(*function)(t_mgr *, char *);
}				t_dtab_wopt;

typedef struct	s_dtab
{
	char		*name;
	int			(*function)(t_mgr *);
}				t_dtab;

int				dtab_loop(t_mgr *mgr, char *opt, t_dtab *tab);
int				dtab_wopt_loop(t_mgr *mgr, char *arg, char *opt,
								  t_dtab_wopt *tab);
int				handle_ip(t_targetset *targets, char *input);
int				handle_port(t_portset *set, char *input);
int				parse_ip(in_addr_t *ip, char *input);
int				parse_ip_file(t_targetset *set, char *filename);
int				parse_opts(t_mgr *mgr, int ac, char **args);
int				parse_port(uint16_t *port, char *input);
int				parse_time(uint32_t *opt_time, char *input);
int				parse_worker(t_workerset *set, char *input);
int				parse_worker_file(t_workerset *set, char *filename);
int				parse_cidr_mask(in_addr_t *subn_m, char *cidr_str);
void			set_ip4range(t_ip4rng *data, const in_addr_t *ip, const in_addr_t *subn_m);
int				add_range_portset(t_portset *set, char **range);

/*
**	boolean user opts
*/
int				h_ack_scan(t_mgr *mgr);
int				h_append_output(t_mgr *mgr);
int				h_bad_checksum(t_mgr *mgr);
int				h_dont_randomize_ports(t_mgr *mgr);
int				h_fin_scan(t_mgr *mgr);
int				h_icmp_host_discovery(t_mgr *mgr);
int				h_list_scan(t_mgr *mgr);
int				h_nmask_host_discovery(t_mgr *mgr);
int				h_norm_output(t_mgr *mgr);
int				h_null_scan(t_mgr *mgr);
int				h_ping_scan(t_mgr *mgr);
int				h_show_only_open_ports(t_mgr *mgr);
int				h_skip_host_discovery(t_mgr *mgr);
int				h_syn_scan(t_mgr *mgr);
int				h_tstamp_host_discovery(t_mgr *mgr);
int				h_traceroute(t_mgr *mgr);
int				h_udp_scan(t_mgr *mgr);
int				h_xmas_scan(t_mgr *mgr);

/*
**				user opts with specification
*/
int				h_ack_portset(t_mgr *mgr, char *input); /*done*/
int				h_daemon(t_mgr *mgr, char *input);
int				h_custom_payload_hex(t_mgr *mgr, char *input);
int				h_custom_random_payload(t_mgr *mgr, char *input); /*done*/
int				h_custom_payload_ascii(t_mgr *mgr, char *input); /*done*/
int				h_exclude_targets(t_mgr *mgr, char *input);
int				h_exclude_target_file(t_mgr *mgr, char *input);
int				h_exclude_ports(t_mgr *mgr, char *input); /*done*/
int				h_fragment_mtu(t_mgr *mgr, char *input);
int				h_host_timeout(t_mgr *mgr, char *input);
int				h_custom_ip_ttl(t_mgr *mgr, char *input);
int				h_target_file(t_mgr *mgr, char *input);
int				h_max_hostgroup(t_mgr *mgr, char *input);
int				h_max_packet_rate(t_mgr *mgr, char *input);
int				h_max_retries(t_mgr *mgr, char *input);
int				h_max_rtt_timeout(t_mgr *mgr, char *input);
int				h_min_hostgroup(t_mgr *mgr, char *input);
int				h_min_packet_rate(t_mgr *mgr, char *input);
int				h_min_rtt_timeout(t_mgr *mgr, char *input);
int				h_scan_portset(t_mgr *mgr, char *input);
int				h_scan_delay(t_mgr *mgr, char *input);
int				h_max_scan_delay(t_mgr *mgr, char *input);
int				h_spoof_srcip(t_mgr *mgr, char *input);
int				h_spoof_srcport(t_mgr *mgr, char *input);
int				h_syn_portset(t_mgr *mgr, char *input);
int				h_udp_portset(t_mgr *mgr, char *input);
int				h_thread_amt(t_mgr *mgr, char *input);
int				h_worker(t_mgr *mgr, char *input);
int				h_worker_file(t_mgr *mgr, char *input);


#endif
