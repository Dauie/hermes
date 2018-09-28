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

# include "../libhermes/incl/libhermes.h"
# include "parser.h"

t_ip4			*new_ip4(void);
int				ip4_cmp(void *left, void *right);
void			*ip4_min(t_node *tree);
uint32_t		ip4_increment(uint32_t ip, int increase);
uint32_t		ip4_decrement(uint32_t ip, int decrease);
uint32_t		ip4_diff(uint32_t left, uint32_t right);

t_ip4range		*new_ip4range(void);
int				ip4rng_cmp(void *ipr_left, void *ipr_right);
int				ip4rng_overlap_cmp(void *left, void *right);
void			*ip4rng_min(t_node *tree);

t_port			*new_port(void);
int				port_cmp(void *prt_left, void *prt_right);
void			*port_min(t_node *tree);

t_portrange		*new_portrange(void);
int				portrng_cmp(void *prt_left, void *prt_right);
int				portrng_overlap_cmp(void *prt_left, void *prt_right);
void			*portrng_min(t_node *tree);

t_worker		*new_worker(void);
int				worker_cmp(void *wrk_left, void *wrk_right);
void			*worker_min(t_node *tree);

int				sanity_check(t_job *job);
int				do_target_exclusions(t_targetlist *targets,
										t_targetlist *exclude);
int				worker_daemon(int port);

#endif
