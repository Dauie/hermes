#include "../../incl/scan.h"

void	scan_port_syn(t_ip4 *target, t_tcp *hdr)
{
	int		sockfd;

	hdr->syn &= 1;
	if ((sockfd = socket(AF_INET, SOCK_RAW, 0)) < 0)
		hermes_error(FAILURE, "socket()");
	/*
	 * if (tcp_syn() > 0)
	 * 		if (tcp_syn_ack() > 0)
	 * 			tcp_rst();
	 */
}