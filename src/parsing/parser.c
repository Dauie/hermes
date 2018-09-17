#include "../../incl/hermes.h"
#include "../../incl/parser.h"

/*
**	If something is commented out in DTABs, it is low priority.
*/

# define DTAB_ENTRIES 17

t_dtab g_dispatch[] = {
		{ "--append-output", h_append_output },
		{ "--badsum", h_bad_checksum },
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
//		{ "-sV", h_service_detection },
		{ "-sX", h_xmas_scan },
		{ "--trace", h_traceroute },
//		{ "-v", h_verbose_output },
//		{ "-vv", h_very_verbose_output },
};

# define DTAB_WOPT_ENTRIES 29

t_dtab_wopt g_dispatch_wopt[] = {

		{ "--data", h_custom_payload_hex },
		{ "--data-length", h_custom_random_payload },
		{ "--data-string", h_custom_payload_ascii },
		{ "--exclude", h_exclude_targets },
		{ "--exclude-file", h_exclude_target_file },
		{ "--exclude-ports", h_exclude_ports },
		{ "-g", h_spoof_srcport},
		{ "--host-timeout", h_host_timeout },
		{ "-iL", h_ip_file },
		{ "--max-hostgroup", h_max_hostgroup },
		{ "--max-rate", h_max_packet_rate },
		{ "--max-retires", h_max_retries },
		{ "--max-rtt-timeout", h_max_rtt_timeout },
		{ "--max-scan-delay", h_max_scan_delay },
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
		{ "--worker", h_worker }
};


int			parse_opts(t_job * job, int ac, char ** args)
{
	int		i;

	i = 0;
	while (++i < ac)
	{
		if (args[i][0] == '-')
		{
			if (dtab_handle(job, *args, g_dispatch) == FAILURE)
			{
				if (dtab_wopt_handle(job, args[i], args[i + 1], g_dispatch_wopt) == FAILURE)
				{
					dprintf(STDERR_FILENO, "hermes: invalid option %s", args[i]);
					exit(EXIT_FAILURE);
				}
				i++;
			}
		}
		else
			parse_ip(job->targets, *args);
	}
	return (0);
}
