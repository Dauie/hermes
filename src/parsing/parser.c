//#include "nmap.h"

int parse_opts(t_ * workload, int ac, char ** args)
{
	/* receive array of args
	 *
	 */

    int     i;
    char *  opt;

    i = 0;
    /* iter arg count */
    while (i < ac)
    {
        /* if we find a (-) flag at the start of
         * the argument
         */
        if (strchr('-', **args) == **args)
        {
            /* set option equal to the argument */
            opt = *args++;
            /* check if option requires parameters */
            if (handle(workload, opt, NULL))
                /* while we do not see a (-) flag */
                while (strchr('-', **args) != **args)
                    /* handle the argument and check
                     * for errors
                     */
                    if (handle(workload, opt, *args++) < 0)
                        //FAILURE
                        return (-1);
        }
    }
    return (0);
}

t_scanjob * parser(t_scanjob * workload, int ac, char ** args) // TODO : args
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
    t_scanjob workload;

    ex = parse_opts(workload, ac, av);
    printf("%d\n", ex);
    return (0);
}

END MAIN */