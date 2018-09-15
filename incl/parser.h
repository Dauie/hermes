#ifndef HERMESE_PARSER_H
# define HERMESE_PARSER_H

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

int handle(char *opt, t_tab *TAB, char *arg, t_job *workload);


/*
**	boolean user options
*/
void			h_append_output(t_job *job);
void			h_show_only_open(t_job *job);
void			h_icmp_host_discovery(t_job *job);
void			h_skip_host_discovery(t_job *job);
void			h_tstamp_host_discovery(t_job *job);
void			h_nmask_host_discovery(t_job *job);
void			h_dont_randomize_ports(t_job *job);
void			h_ack_scan(t_job *job);
void			h_fin_scan(t_job *job);
void			h_scan_list(t_job *job);
void			h_ping_scan(t_job *job);
void			h_syn_scan(t_job *job);
void			h_null_scan(t_job *job);
void			h_udp_scan(t_job *job);
void			h_xmas_scan(t_job *job);
void			h_traceroute(t_job *job);

/*
**	user options with specification
*/
void 			h_ack_portlist(t_job *job, char *input);
void 			h_bad_checksum(t_job *job, char *input);
void 			h_custom_payload_hex(t_job *job, char *input);
void 			h_custom_random_payload(t_job *job, char *input);
void 			h_custom_payload_ascii(t_job *job, char *input);
void 			h_exclude_targes(t_job *job, char *input);
void 			h_exclude_target_file(t_job *job, char *input);
void 			h_exclude_ports(t_job *job, char *input);
void 			h_fragment_mtu(t_job *job, char *input);
void 			h_host_timeout(t_job *job, char *input);
void 			h_ip_ttl(t_job *job, char *input);
void 			h_ip_file(t_job *job, char *input);
void 			h_max_hostgroup(t_job *job, char *input);
void 			h_max_packet_rate(t_job *job, char *input);
void 			h_max_retries(t_job *job, char *input);
void 			h_max_rtt_timeout(t_job *job, char *input);
void 			h_min_hostgroup(t_job *job, char *input);
void 			h_min_packet_rate(t_job *job, char *input);
void 			h_min_rtt_timeout(t_job *job, char *input);
void 			h_norm_output(t_job *job, char *input);
void 			h_scan_portlist(t_job *job, char *input);
void 			h_scan_delay(t_job *job, char *input);
void 			h_spoof_srcip(t_job *job, char *input);
void 			h_spoof_srcport(t_job *job, char *input);
void 			h_syn_portlist(t_job *job, char *input);
void 			h_udp_portlist(t_job *job, char *input);
void 			h_thread_amt(t_job *job, char *input);
void 			h_worker(t_job *job, char *input);

#endif