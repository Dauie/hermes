#ifndef HERMES_BINNIFY_H
#define HERMES_BINNIFY_H

#include "../incl/job.h"
#include "../binn/src/binn.h"

binn		*binnify_job(t_job *job);
void		unbinnify_job(binn *obj, t_job *job);


#endif //FT_NMAP_BINNIFY_H
