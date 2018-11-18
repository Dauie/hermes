#ifndef HERMES_DEFINES
# define HERMES_DEFINES

# define WKRMGR_POLL_TIMEO (250)

# define IP_HDRLEN (20)
# define TCP_HDR_LEN (20)
# define UDP_HDR_LEN (8)
# define DEF_TRAN_HDRLEN (20)

# define TTL_MAX 256
# define MTU_MAX 1500
# define PORT_MAX (65535)
# define CUST_PAYLOAD_MAX (64)
# define THREAD_MAX (256)
# define WKR_HSTGRP_MAX (4096)
# define THRD_HSTGRP_MAX (32)
# define HSTGRP_MIN (1)

# ifndef fast_swap_ints
# define fast_swap_ints(x, y) ((x ^= y),(y ^= x),(x ^= y))
# endif

#ifndef BYTEORDR_SUM
#define BYTEORDR_SUM(x, y) ((htonl(ntohl(x) + ntohl(y))))
#endif

# define DEF_TCP_PROBE_PORT (80)
# define DEF_SCTP_PROBE_PORT (80)
# define DEF_UDP_PROBE_PORT (40125)

# define DEF_MAX_RETRIES (10)
# define DEF_INIT_SCAN_DELAY (0)
# define DEF_SCAN_DELAY (1000)
# define DEF_MIN_RTT_TIMEOUT (100) /* ms */
# define DEF_INIT_RTT_TIMEOUT (1000) /* ms */
# define DEF_MAX_RTT_TIMEOUT (10000) /* ms */

# define STAT_OPEN (0)
# define STAT_CLOSED (1)
# define STAT_FILTERED (2)
# define STAT_UNFILTERED (3)
# define STAT_OPENFILTERED (4)
# define STAT_CLOSEDFILTERED (5)

/*
**	Quick Access to Node Types
*/

#ifndef WORKER_DATA
#define WORKER_DATA(x) ((t_wrkr*)x->workers->wrkrs->data)
#endif

#ifndef IP4RNG_DATA
#define IP4RNG_DATA(x) ((t_ip4rng*)x->iprngs->data)
#endif

#endif