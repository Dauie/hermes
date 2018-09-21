#include "../../incl/hermes.h"
#include "../../incl/parser.h"

/*
**	If something is commented out in DTABs, it is low priority.
*/

# define DTAB_ENTRIES 18

t_dtab g_disp[] = {
		{ "--append-output", h_append_output },
		{ "--badsum", h_bad_checksum },
		{ "--open", h_show_only_open_ports },
//		{ "--iflist", h_show_interfaces },
//		{ "-O", h_os_detection },
		{ "-oN", h_norm_output},
		{ "-PE", h_icmp_host_discovery },
		{ "-Pn", h_skip_host_discovery },
		{ "-PP", h_tstamp_host_discovery },
		{ "-PM", h_nmask_host_discovery },
		{ "-r", h_dont_randomize_ports },
		{ "-sA", h_ack_scan },
		{ "-sF", h_fin_scan },
		{ "-sL", h_list_scan },
		{ "-sn", h_ping_scan },
		{ "-sS", h_syn_scan },
		{ "-sT", h_null_scan },
		{ "-sU", h_udp_scan },
//		{ "-sV", h_service_detection },
		{ "-sX", h_xmas_scan },
		{ "--trace", h_traceroute },
//		{ "-v", h_verbose_output },
//		{ "-vv", h_very_verbose_output },
};

# define DTAB_WOPT_ENTRIES 29

t_dtab_wopt g_disp_wopt[] = {

		{ "--data", h_custom_payload_hex },
		{ "--data-length", h_custom_random_payload },
		{ "--data-string", h_custom_payload_ascii },
		{ "--exclude", h_exclude_targets },
		{ "--excludefile", h_exclude_target_file },
		{ "--exclude-ports", h_exclude_ports },
		{ "-g", h_spoof_srcport},
		{ "--host-timeout", h_host_timeout },
		{ "-iL", h_target_file },
		{ "--max-hostgroup", h_max_hostgroup },
		{ "--max-rate", h_max_packet_rate },
		{ "--max-retires", h_max_retries },
		{ "--max-rtt-timeout", h_max_rtt_timeout },
		{ "--max-scan-delay", h_max_scan_delay },
		{ "--min-hostgroup", h_min_hostgroup },
		{ "--min-rate", h_min_packet_rate },
		{ "--min-rtt-timeout", h_min_rtt_timeout },
		{ "--fragment-mtu", h_fragment_mtu },
//		{ "-oX", h_xml_output},
		{ "-p", h_scan_portlist },
		{ "-PA", h_ack_portlist },
		{ "-PS", h_syn_portlist },
		{ "-PU", h_udp_portlist },
//		{ "--resume", h_resume_output },
		{ "-S", h_spoof_srcip },
		{ "--scan-delay", h_scan_delay },
		{ "--source-port", h_spoof_srcport },
		{ "-ttl", h_custom_ip_ttl },
		{ "--thread", h_thread_amt },
		{ "--worker", h_worker },
		{ "--worker-file", h_worker_file }
};

int			dtab_loop(t_job *job, char *arg, t_dtab *tab)
{
	int		i;
	size_t	len;

	i = -1;
	len = strlen(arg);
	/* iterate TAB and look for the correct entry */
	while (++i < DTAB_ENTRIES)
	{
		if (strncmp(arg, tab[i].name, len) == 0)
		{
			tab[i].function(job);
			return (SUCCESS);
		}
	}
	return (FAILURE);
}

int			dtab_wopt_loop(t_job *job, char *arg, char *opt,
							  t_dtab_wopt *tab)
{
	int		i;
	size_t	len;

	i = -1;
	len = strlen(arg);
	/* iterate TAB and look for the correct entry */
	while (++i < DTAB_WOPT_ENTRIES)
	{
		if (strncmp(arg, tab[i].name, len) == 0)
		{
			tab[i].function(job, opt);
			return (SUCCESS);
		}
	}
	return (FAILURE);
}

int			parse_opts(t_job * job, int ac, char **args)
{
	int		i;

	i = 0;
	while (++i < ac)
	{
		if (args[i][0] == '-') {
			if (dtab_loop(job, args[i], g_disp) == FAILURE)
			{
				if (dtab_wopt_loop(job, args[i], args[i + 1], g_disp_wopt) == FAILURE)
					hermes_error(INPUT_ERROR, TRUE, 2, "invalid option", args[i]);
				i++; 
			}
		}
		else
		{
			if (handle_ip(&job->targets, args[i]) < 0)
				hermes_error(INPUT_ERROR, TRUE, 1, "issue parsing targets");
		}
	}
	return (SUCCESS);
}
