#include "../incl/hermes.h"




int			hermes_arp(in_addr_t addr, uint8_t *macbuff)
{
	int		sock;

	if ((sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP)) < 0))
		return (hermes_error(FAILURE, "socket() AF_PACKET %s", strerror(errno)));

}