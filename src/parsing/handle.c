#include "../../hermes.h"

int handle(t_manager *  workload,
                   char *       opt,
                   char *       arg)
{
    return (resolve_handler(opt, &DTAB_ENTRIES, arg, workload))
}
