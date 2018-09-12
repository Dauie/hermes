#ifndef HANDLERS_H
# define HANDLERS_H

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
**	If something is commented out in DTABs, it is low priority.
*/

# define DTAB_ENTRIES 16

t_tab g_dispach[DTAB_ENTIRES] = {
		{ "--append-output", h_append_output },
		{ "--open", h_show_only_open },
//		{ "--iflist", h_show_interfaces },
//		{ "-O", h_os_detection },
		{ "-PE", h_icmp_host_discovery },
		{ "-Pn", h_skip_host_discovery },
		{ "-PP", h_tstamp_host_discovery },
		{ "-PM", h_nmask_host_discovery },
		{ "-r", h_dont_randomize_ports },
		{ "-sA", h_ack_scan },
		{ "-sF", h_fin_scan },
		{ "-sL", h_scan_list },
		{ "-sn", h_ping_scan },
		{ "-sS", h_syn_scan },
		{ "-sT", h_null_scan },
		{ "-sU", h_udp_scan },
		{ "-sV", h_service_detection },
		{ "-sX", h_xmas_scan },
		{ "--trace", h_traceroute },
//		{ "-v", h_verbose_output },
//		{ "-vv", h_very_verbose_output },
};

# define DTAB_WOPT_ENTRIES 27

t_tab g_dispatch_wopt[DTAB_ENTRIES] = {
		{ "--badsum", h_bad_checksum },
		{ "--data", h_custom_payload_hex },
		{ "--data-length", h_custom_random_payload },
		{ "--data-string", h_custom_payload_ascii },
		{ "--exclude", h_exclude_targes },
		{ "--exclude-file", h_exclude_target_file },
		{ "--exclude-ports", h_exclude_ports },
		{ "-g", h_spoof_srcport},
		{ "--host-timeout", h_host_timeout },
		{ "-iL", h_ip_file },
		{ "--max-hostgroup", h_max_hostgroup },
		{ "--max-rate", h_max_packet_rate },
		{ "--max-retires", h_max_retries },
		{ "--max-rtt-timeout", h_max_rtt_timeout },
		{ "--min-hostgroup", h_min_hostgroup },
		{ "--min-rate", h_min_packet_rate },
		{ "--min-rtt-timeout", h_min_rtt_timeout },
		{ "--mtu", h_fragment_mtu },
		{ "-oN", h_norm_output},
//		{ "-oX", h_xml_output},
		{ "-p", h_scan_portlist },
		{ "-PA", h_ack_portlist },
		{ "-PS", h_syn_portlist },
		{ "-PU", h_udp_portlist },
//		{ "--resume", h_resume_output },
		{ "-S", h_spoof_srcip },
		{ "--scan-delay", h_scan_delay },
		{ "--source-port", h_spoof_srcport },
		{ "-ttl", h_ip_ttl },
		{ "--thread", h_thread_amt },
		{ "--worker", h_worker },
};

/* LEXicon for
 * file parsing
 */

# define LEX_ENTRIES 1

t_tab LEX[LEX_ENTRIES][] = {
		{ "#IP", NULL }
};


#endif