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
# include "job.h"

int				sanity_check(t_job *job);

t_ip4			*new_ip4(void);
t_ip4range		*new_ip4range(void);
t_port			*new_port(void);
t_portrange		*new_portrange(void);
t_worker		*new_worker(void);


int				ip4_cmp(void *ip_left, void *ip_right);
void			*ip4_min(t_node *tree);
int				ip4range_cmp(void *ipr_left, void *ipr_right);
void			*ip4range_min(t_node *tree);
int				port_cmp(void *prt_left, void *prt_right);
void			*port_min(t_node *tree);
int				portrange_cmp(void *prt_left, void *prt_right);
void			*portrange_min(t_node *tree);
int				worker_cmp(void *wrk_left, void *wrk_right);
void			*worker_min(t_node *tree);
int				do_exclusions(t_targetlist *targets, t_targetlist *exclude);

#endif
