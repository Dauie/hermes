#ifndef PARSER_H
# define PARSER_H

# include "job.h"
# include "hermes.h"
# include "defined.h"

/*
**	Dispatch TABle
*/

typedef struct	s_dtab_wopt
{
	char		*name;
	void		(*function)(t_job *, char *);
}				t_dtab_wopt;

typedef struct	s_dtab
{
	char		*name;
	void		(*function)(t_job *);
}				t_dtab;


/*
**	LEXicon for
**	file parsing
*/

//# define LEX_ENTRIES 1
//
//t_tab LEX[LEX_ENTRIES][] = {
//		{ "#IP", NULL }
//};

int				dtab_loop(t_job *job, char *opt, t_dtab *tab);
int				dtab_wopt_loop(t_job *job, char *arg, char *opt,
								  t_dtab_wopt *tab);
int				parse_port(uint16_t *port, char *port_str);
int				handle_port(t_portlist *list, char *input);
int				parse_time(uint32_t *opt_time, char *input);
int				parse_opts(t_job * job, int ac, char ** args);
int				handle_ip(t_targetlist *targets, char *arg);
int				parse_ip(uint32_t *ip, char *ip_str);
int				parse_ip_file(t_targetlist *list, char *filename);
int				handle_ip(t_targetlist *targets, char *arg);

/*
**	boolean user options
*/
void			h_ack_scan(t_job *job);
void			h_append_output(t_job *job);
void			h_bad_checksum(t_job *job);
void			h_dont_randomize_ports(t_job *job);
void			h_fin_scan(t_job *job);
void			h_icmp_host_discovery(t_job *job);
void			h_list_scan(t_job *job);
void			h_nmask_host_discovery(t_job *job);
void			h_norm_output(t_job *job);
void			h_null_scan(t_job *job);
void			h_ping_scan(t_job *job);
void			h_show_only_open_ports(t_job *job);
void			h_skip_host_discovery(t_job *job);
void			h_syn_scan(t_job *job);
void			h_tstamp_host_discovery(t_job *job);
void			h_traceroute(t_job *job);
void			h_udp_scan(t_job *job);
void			h_xmas_scan(t_job *job);

/*
**	user options with specification
*/
void			h_ack_portlist(t_job *job, char *input); /*done*/
void			h_custom_payload_hex(t_job *job, char *input);
void			h_custom_random_payload(t_job *job, char *input); /*done*/
void			h_custom_payload_ascii(t_job *job, char *input); /*done*/
void			h_exclude_targets(t_job *job, char *input);
void			h_exclude_target_file(t_job *job, char *input);
void			h_exclude_ports(t_job *job, char *input); /*done*/
void			h_fragment_mtu(t_job *job, char *input);
void			h_host_timeout(t_job *job, char *input);
void			h_custom_ip_ttl(t_job *job, char *input);
void			h_target_file(t_job *job, char *input);
void			h_max_hostgroup(t_job *job, char *input);
void			h_max_packet_rate(t_job *job, char *input);
void			h_max_retries(t_job *job, char *input);
void			h_max_rtt_timeout(t_job *job, char *input);
void			h_min_hostgroup(t_job *job, char *input);
void			h_min_packet_rate(t_job *job, char *input);
void			h_min_rtt_timeout(t_job *job, char *input);
void			h_scan_portlist(t_job *job, char *input);
void			h_scan_delay(t_job *job, char *input);
void			h_max_scan_delay(t_job *job, char *input);
void			h_spoof_srcip(t_job *job, char *input);
void			h_spoof_srcport(t_job *job, char *input);
void			h_syn_portlist(t_job *job, char *input);
void			h_udp_portlist(t_job *job, char *input);
void			h_thread_amt(t_job *job, char *input);
void			h_worker(t_job *job, char *input);

#endif
