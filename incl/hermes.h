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

int				sanity_check(t_job *job);

t_ip4			*new_ip4(void);
t_ip4range		*new_ip4range(void);
t_port			*new_port(void);
t_portrange		*new_portrange(void);
t_worker		*new_worker(void);

void			ip4_add(uint32_t *ip, int increase);
int				ip4_cmp(void *left, void *right);
void			*ip4_min(t_node *tree);
int				ip4rng_cmp(void *ipr_left, void *ipr_right);
int				ip4rng_overlap_cmp(void *left, void *right);
void			*ip4rng_min(t_node *tree);
int				port_cmp(void *prt_left, void *prt_right);
void			*port_min(t_node *tree);
int				portrng_cmp(void *prt_left, void *prt_right);
int				portrng_overlap_cmp(void *prt_left, void *prt_right);
void			*portrng_min(t_node *tree);
int				worker_cmp(void *wrk_left, void *wrk_right);
void			*worker_min(t_node *tree);
int				do_exclusions(t_targetlist *targets, t_targetlist *exclude);


int						worker_daemon(int port);

#endif
