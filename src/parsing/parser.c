#include "../../hermes.h"

int parse_opts(t_job * workload, int ac, char ** args)
{
	/* receive array of args
	 *
	 */

    int     i;
    int     e;
    int     len;
    char *  opt;

    i = -1;
    e = DTAB_ENTRIES;
    /* iter arg count */
    while (args++ && ++i < ac)
        /*
         * TODO: debate using memory safe functions
         * with less efficiency than str function
         * with faster efficiency
         */
        len = strlen(*args);
        /* if we find a (-) flag at the start of the argument */
        if (memchr(*args, '-', len) == *args)
        {
            /* set option equal to the argument */
            opt = *args;
            args++;
            /* check if option requires parameters */
            if (handle(opt, &DTAB, NULL, workload, e) == 1)
                /* while we do not see a (-) flag */
                while (memchr(*args, '-', len) != *args && i++ < ac)
                    /* handle the argument and check for errors */
                    if (handle(opt, &DTAB, args++, workload, e) < 0)
                        //FAILURE
                        //print and exit
                        return (-1);
        }

    return (0);
}

t_job * parser(t_job * workload, int ac, char ** args) // TODO : args
{
	/* parser() takes two parameters:
	 *  @p workload is a pointer to a struct of type
	 *      scanjob. workload contains a struct of
	 *      flags to be set.
	 *
	 *  @p args contains arguments passed to the
	 *      program.
	 *
	 *  --------------------------------------------
	 *
	 *  the args are parsed into tuple-like options,
	 *      delimited by a '-' in front of each flag
	 *      argument, e.x. "-O" (os-detection)
	 *
	 *  the opts are then iterated and are passed
	 *      to a dispatch table for verification and
	 *      processing
	 */

	if (parse_opts(workload, ac, args) < 0)
        help(NULL, NULL);
	return (workload);
}

/* TEST MAIN

int main(int ac, char **av)
{
    int     i;
    char ** ex;
    t_job workload;

    ex = parse_opts(workload, ac, av);
    printf("%d\n", ex);
    return (0);
}

END MAIN */