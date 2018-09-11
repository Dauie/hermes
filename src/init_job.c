
#include "../incl/hermese.h"
#include "../incl/ops_defaults.h"
#include "../incl/bool.h"
#include "../incl/job.h"

/*
** TODO: Right now ports must be specified by user, popular ports will be added later.
** TODO:
*/


static void default_discops(t_disc_ops *disc, t_portlists *portlists)
{
/*
**  If no host discovery options are given, Nmap sends a TCP ACK packet destined
**  for port 80 and an ICMP echo request query to each target machine. An exception
**  to this is that an ARP scan is used for any targets which are on a local
**  ethernet network. For unprivileged Unix shell users, a SYN packet is sent
**  instead of the ACK using the connect system call.
*/
	disc->echo = TRUE;
	disc->ack = TRUE;
	if (!(portlists->disc_ack_portlist = malloc(sizeof(uint16_t))))
		hermese_error("Error malloc()", errno);
	portlists->disc_ack_portlist->port_count = 1;
	portlists->disc_ack_portlist->ports[0] = 80;
}

/* TODO: All DEFINED settings are in Milisecond, but timeval saves in Microseconds, we need to do conversion */
/* TODO: There should possibly be a way to configure max scan delay for TCP and UDP individually */
static void default_timeops(t_time_ops *time)
{
	time->host_timeo.tv_usec = 0; /* TODO: Find default host timeout */
	time->init_rtt_timeo.tv_usec = INITIAL_RTT_TIMEOUT;
	time->max_rtt_timeo.tv_usec = MAX_RTT_TIMEOUT;
	time->min_rtt_timeo.tv_usec = MIN_RTT_TIMEOUT;
	time->max_scan_delay.tv_usec = MAX_SCAN_DELAY;
	time->scan_delay.tv_usec = 0; /* TODO: Find default scan delay time */
}

static void default_rateops(t_rate_ops *rate)
{
	rate->min_packet_rate = 0.0; /* By default packet rate is not set, and will rise and fall based on network performance */
	rate->max_packet_rate = 0.0;
	rate->max_retries = 10; /* TODO: this is a guess by Wyatt, subject to change */
	rate->min_hostgroup = 5; /* TODO: again, this is a guess. */
	rate->max_hostgroup = 4096; /*TODO: guess */
	rate->min_parallelism = 0; /* By default both parallelism options are 0, and will be set based on network performance */
	rate->max_parallelism = 0;
	rate->icmp_ratelimit = 0; /* ICMP Rate limit will always be zero by default, only used for UDP Scan */
	rate->rst_ratelimit = 0;  /* RST Rate limit will always be zero by default, only used with SYN Scan */
}


