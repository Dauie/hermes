#include "../../incl/hermes.h"
#include "../../incl/parser.h"


int				h_list_scan(t_mgr *mgr)
{
	mgr->job.opts.bitops.do_list_scan = true;
	return (SUCCESS);
}

int				h_ping_scan(t_mgr *mgr)
{
	mgr->job.opts.bitops.do_ping_scan = true;
	return (SUCCESS);
}

int				h_syn_scan(t_mgr *mgr)
{
	mgr->job.opts.bitops.do_syn_scan = true;
	return (SUCCESS);
}

int				h_ack_scan(t_mgr *mgr)
{
	mgr->job.opts.bitops.do_ack_scan = true;
	return (SUCCESS);
}

int				h_null_scan(t_mgr *mgr)
{
	mgr->job.opts.bitops.do_null_scan = true;
	return (SUCCESS);
}

int				h_fin_scan(t_mgr *mgr)
{
	mgr->job.opts.bitops.do_fin_scan = true;
	return (SUCCESS);
}

int				h_xmas_scan(t_mgr *mgr)
{
	mgr->job.opts.bitops.do_xmas_scan = true;
	return (SUCCESS);
}

int				h_udp_scan(t_mgr *mgr)
{
	mgr->job.opts.bitops.do_udp_scan = true;
	return (SUCCESS);
}

int				h_bad_checksum(t_mgr *mgr)
{
	mgr->job.opts.bitops.do_bad_checksum = true;
	return (SUCCESS);
}

int				h_custom_ip_ttl(t_mgr *mgr, char *input)
{
	int			ttl;

	if (!input)
		return (hermes_error(EXIT_FAILURE, "-ttl not specified"));
	if ((ttl = atoi(input)) <= 0 || ttl > TTL_MAX)
		return (hermes_error(EXIT_FAILURE, "bad ttl specified"));
	mgr->job.opts.ip_ttl = (uint8_t)ttl;
	return (SUCCESS);
}

int				h_fragment_mtu(t_mgr *mgr, char *input)
{
	int			mtu;

	if (!input)
		return (hermes_error(EXIT_FAILURE, "--fragment-mtu not specified"));
	if ((mtu = atoi(input)) <= 0 || mtu > MTU_MAX)
		return (hermes_error(EXIT_FAILURE, "bad fragment-mtu specified"));
	mgr->job.opts.fragment_mtu = (uint8_t)mtu;
	return (SUCCESS);
}

int				h_spoof_srcip(t_mgr *mgr, char *input)
{
	uint32_t	ip;

	if (!input)
		return (hermes_error(EXIT_FAILURE, "spoof ips address not specified"));
	if (parse_ip(&ip, input) < 0)
		return (hermes_error(EXIT_FAILURE, "bad spoof ips address specified"));
	mgr->job.opts.spoofed_srcaddr = ip;
	return (SUCCESS);
}

int				h_spoof_srcport(t_mgr *mgr, char *input)
{
	uint16_t	port;

	if (!input)
		return (hermes_error(EXIT_FAILURE, "spoof ports not specified"));
	if (parse_port(&port, input) == FAILURE)
		return (hermes_error(EXIT_FAILURE, "bad spoof ports specified"));
	mgr->job.opts.spoofed_srcport = port;
	return (SUCCESS);
}

int				h_exclude_targets(t_mgr *mgr, char *input)
{
	if (!input)
		return (hermes_error(EXIT_FAILURE, "--exclude-targets not specified"));
	if (!mgr->exclude_targets)
		mgr->exclude_targets = new_targetset();
	if (handle_ip(mgr->exclude_targets, input) == FAILURE)
		return (hermes_error(EXIT_FAILURE, "bad exclude target ips(s)"));
	return (SUCCESS);
}
