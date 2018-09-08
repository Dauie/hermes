#include "../../hermes.h"

int h_file(t_job * workload, char * args)
{
    int     r;
    int     fd;
    int     len;
    char *  opt;
    char *  line;

    if (args == NULL) return (1);

    if ((fd = open(args, O_RDONLY)) < 0)
        return (fd);

    r = 1;
    while ((r = get_next_line(fd, &line)) > 0)
        /*
         * TODO: debate using memory safe functions
         * with less efficiency than str function
         * with more efficiency
         */
        len = strlen(line);
        if (memchr(line, '#', len) == line)
            /* set option equal to the file header */
            opt = line;
	else
            /* check if option requires parameters */
            if (handle(opt, &LEX, NULL, workload, LEX_ENTRIES) == 1)
	    {
                /* if we do not see a (#) flag */
            	if (memchr(line, '#', len) != line)
                    /* handle the argument and check
                     * for errors
                     */
                    if (handle(opt, &LEX, line, workload, LEX_ENTRIES) < 0)
                        //FAILURE
                        //print and exit
                        return (-1);
		else
			/* we know we need an argument but we don't have one */
			return (-1);
	    }

    if (line) free(line);
    close(fd);

    if (r < 0)
        return (r);
    return (0);
}
