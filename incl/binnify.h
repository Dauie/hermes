#ifndef HERMES_BINNIFY_H
#define HERMES_BINNIFY_H

#include "../incl/job.h"
#include "../binn/src/binn.h"

binn				*binnify_opts(t_opts *opts);
binn				*binnify_portset(t_portset *set);
binn				*binnify_targetset(t_targetset *set);
void				unbinnify_opts(t_opts *opts, binn *obj);
void				unbinnify_portset(t_portset *set, binn *obj);
void				unbinnify_targetset(t_targetset *set, binn *obj);

#endif //HERMES_BINNIFY_H
