#include "../../hermes.h"

int handle(char *opt, t_tab *TAB, char *arg, t_job *workload)
{
    int     i;
    int     len;
    char *  result;

    i = -1;
    result = NULL;
    len = strlen(opt);
    /* iterate TAB and look for the correct entry */
    while (TAB[++i].name && result == NULL)
        result = strnstr(opt, TAB[i].name, len);
    if (result)
    {
        if (arg != NULL)
            return (TAB[i].function(workload, arg));
        else
            return (TAB[i].function(workload));
    }

    return (-1);
}
