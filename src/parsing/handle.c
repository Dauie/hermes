#include "../../hermes.h"

int handle(char *   opt,
           t_tab *  TAB,
           char *   arg,
           t_job *  workload,
           int tab_entries)
{
    int i;
    int len;

    i = -1;
    len = strlen(opt);
    /* iterate TAB and look for the correct entry */
    while (++i < tab_entries && result == NULL)
        result = strnstr(opt, TAB[i].name, len);

    if (result)
    {
        if (&TAB[i].function != NULL)
            return (TAB[i].function(workload, arg));
        else
            return (1);
    }

    return (0);
}
