#include "../../hermes.h"

int isin(char * line, t_tab * TAB, t_job * workload)
{
    int len;

    len = strlen(TAB);

    while (++i < DTAB_ENTRIES && result == NULL)
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

int h_file(t_job * workload, char * args)
{
    int     r;
    int     fd;
    char *  line;

    if (!args) return (1);

    if ((fd = open(args, O_RDONLY)) < 0)
        return (-1);

    r = 1;
    if ((r = get_next_line(fd, &line)) > 0)
    {
        if (isin(line, &LEX, workload)
            while ((r = get_next_line(fd, &line)) > 0)
                if (handle(workload, '-ip') < 0)
                    //invalid IP
                    //print and exit
                    return (-1);
    }
    else
    {
        //FILE is empty
        //print and exit
    }
    if (line) free();
    close(fd);
    if (r < 0)
        return (-1);
    return (1);
}