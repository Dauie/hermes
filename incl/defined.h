#ifndef HERMESE_OPS_DEFAULTS_H
# define HERMESE_OPS_DEFAULTS_H

/*
**   All of these default settings for timeing and rate are borrowed from
**   the original nmap project written by Gordon "Fyodor" Lyon.
**   Im positive that these defaults are product of years of research
**   and trial and error.
*/
# define PORT_MAX (65535)
# define MAX_THREADS (256)
# define MAX_HOSTGROUP (4096)
# define MIN_HOSTGROUP (1)

# define DEFAULT_TCP_PROBE_PORT 80
# define DEFAULT_SCTP_PROBE_PORT 80
# define DEFAULT_UDP_PROBE_PORT 40125

# define MAX_SCAN_DELAY 1000 /* ms */

/*
**   We wait at least 100 ms for a response by default - while that
**   seems aggressive, waiting too long can cause us to fail to detect
**   drops until many probes later on extremely low-latency
**   networks (such as localhost scans).
*/

# define MIN_RTT_TIMEOUT 100 /* ms */
# define INITIAL_RTT_TIMEOUT 1000 /* ms */
# define MAX_RTT_TIMEOUT 10000 /* ms */

# define MAX_RETRANSMISSIONS 10

#endif