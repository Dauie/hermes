#ifndef HERMES_BINNIFY_H
# define HERMES_BINNIFY_H

#ifndef HERMES_H
# include "hermes.h"
#endif

binn				*binnify_job(t_job *job);
binn				*binnify_opts(t_opts *opts);
binn				*binnify_portset(t_portset *set);
binn				*binnify_targetset(t_targetset *set);
void				unbinnify_opts(t_opts *opts, binn *obj);
void				unbinnify_portset(t_portset *set, binn *obj);
void				unbinnify_targetset(t_targetset *set, binn *obj);
void				unbinnify_job(t_job *job, binn *obj);


#endif //HERMES_BINNIFY_H
