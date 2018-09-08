#include "../../hermes.h"

int handle(char *   opt,
           t_tab *  TAB,
           char *   arg,
           t_job *  workload,
           int tab_entries)
{
    int il
    int len;

    i = -1;
    len = strlen(opt)
    while (++i < tab_entries && result == NULL)
        result = memcmp(opt, TAB[i].name);

    if (result)
    {
        if (&TAB[i].function != NULL)
            return (TAB[i].function(workload, arg));
        else
            return (1);
    }

    return (0);
}
