#ifndef HERMES_H
# define HERMES_H

# include <errno.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>

# include <pthread.h>
# include <sys/socket.h>
# include <sys/time.h>

# include <arpa/inet.h>
# include <netinet/ip.h>
# include <netinet/in.h>
# include <netdb.h>
# include <netinet/ip_icmp.h>
# include "../binn/src/binn.h"

# include "../libhermes/incl/libhermes.h"
# include "parser.h"

t_node          *split_prtrng_n(void *data, uint32_t splits);
t_node          *split_ip4rng_n(void *data, uint32_t splits);

t_ip4			*new_ip4(void);
int				ip4_cmp(void *left, void *right);
void			*ip4_min(t_node *tree);
in_addr_t		ip4_increment(in_addr_t ip, uint32_t increase);
in_addr_t		ip4_decrement(in_addr_t ip, uint32_t decrease);
uint32_t		ip4_diff(in_addr_t left, in_addr_t right);

t_ip4rng		*new_ip4range(void);
int				ip4rng_cmp(void *ipr_left, void *ipr_right);
int				ip4rng_overlap_cmp(void *left, void *right);
int				ip4_ip4rng_overlap_cmp(void *ip, void *iprng);
void			*ip4rng_min(t_node *tree);

t_port			*new_port(void);
int				port_cmp(void *prt_left, void *prt_right);
void			*port_min(t_node *tree);

t_prtrng		*new_portrange(void);
int				portrng_cmp(void *prt_left, void *prt_right);
int				port_prtrng_overlap_cmp(void *port, void *prtrng);
int				portrng_overlap_cmp(void *prt_left, void *prt_right);
void			*portrng_min(t_node *tree);

t_worker		*new_worker(void);
int				worker_cmp(void *wrk_left, void *wrk_right);
void			*worker_min(t_node *tree);

int				sanity_check(t_mgr *mgr);
void			do_exclusions(t_mgr *mgr);
int				worker_daemon(int port);

binn			*make_ops_binn(t_ops *options);
binn			*make_portlist_binn(t_portlist *ports);
binn			*make_targetlist_binn(t_targetlist *list);

int             manager(t_mgr *mgr);
t_node          *partition_jobs(t_job *job, uint32_t parts);

#endif
