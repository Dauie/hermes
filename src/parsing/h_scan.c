# include "../../incl/job.h"
# include "../../incl/parser.h"
# include "../../incl/defined.h"

void			h_list_scan(t_job *job)
{
	job->options.bitops.do_list_scan = TRUE;
}

void			h_ping_scan(t_job *job)
{
	job->options.bitops.do_ping_scan = TRUE;
}

void			h_syn_scan(t_job *job)
{
	job->options.bitops.do_syn_scan = TRUE;
}

void			h_ack_scan(t_job *job)
{
	job->options.bitops.do_ack_scan = TRUE;
}

void			h_null_scan(t_job *job)
{
	job->options.bitops.do_null_scan = TRUE;
}

void			h_fin_scan(t_job *job)
{
	job->options.bitops.do_fin_scan = TRUE;
}

void			h_xmas_scan(t_job *job)
{
	job->options.bitops.do_xmas_scan = TRUE;
}

void			h_udp_scan(t_job *job)
{
	job->options.bitops.do_udp_scan = TRUE;
}

void			h_bad_checksum(t_job *job)
{
	job->options.bitops.do_bad_checksum = TRUE;
}

void			h_custom_ip_ttl(t_job *job, char *input)
{
	int			ttl;

	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "-ttl not specified");
	if ((ttl = atoi(input)) <= 0 || ttl > TTL_MAX)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad ttl specified");
	job->options.ip_ttl = (uint16_t)ttl;
	job->options.bitops.custom_ip_ttl = TRUE;
}

void			h_fragment_mtu(t_job *job, char *input)
{
	int			mtu;

	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--fragment-mtu not specified");
	if ((mtu = atoi(input)) <= 0 || mtu > MTU_MAX)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad fragment-mtu specified");
	job->options.fragment_mtu = (uint16_t)mtu;
	job->options.bitops.fragment_pkts = TRUE;
}

void			h_spoof_srcip(t_job *job, char *input)
{
	uint32_t	ip;

	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "spoof ip address not specified");
	if (parse_ip(&ip, input) < 0)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad spoof ip address specified");
	job->options.spoofed_srcaddr = ip;
}

void			h_spoof_srcport(t_job *job, char *input)
{
	uint16_t	port;

	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "spoof port not specified");
	if (parse_port(&port, input) == FAILURE)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad spoof port specified");
	job->options.spoofed_srcport = port;
}

void			h_exclude_targets(t_job *job, char *input)
{
	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--exclude-targets not specified");
	if (handle_ip(&job->exclude_targets, input) == FAILURE)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad exclude target ip(s)");
}
