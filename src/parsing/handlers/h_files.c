#include "../../hermes.h"

int resolve_handler(char * opt, t_tab * TAB, arg, t_job * workload)
{
    int len;

    len = strlen(opt)
    while (++i < DTAB_ENTRIES && result == NULL)
        result = memcmp(opt, TAB[i].name);

    if (result)
    {
        if (&TAB[i].function != NULL)
            return (TAB[i].function(workload, ));
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
    //if (isin(line, &LEX, workload)
    while ((r = get_next_line(fd, &line)) > 0)
        if (strchr('#', line) == **args)
        {
            /* set option equal to the file header */
            opt = line;
            /* check if option requires parameters */
            if (handle(opt, &LEX, NULL, workload) == 1)
                /* while we do not see a (-) flag */
                while (strchr('#', line) != line)
                    /* handle the argument and check
                     * for errors
                     */
                    if (handle(opt, &LEX, line, workload) < 0)
                        //FAILURE
                        return (-1);
        }
    //invalid IP
    //print and exit
    return (-1);

    if (line) free(line);
    close(fd);
    if (r < 0)
        return (-1);
    return (1);
}