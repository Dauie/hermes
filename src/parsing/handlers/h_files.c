# include "../../../incl/job.h"
# include "../../../incl/parser.h"
# include "../../../incl/bool.h"

void			h_append_output(t_job *job)
{
	job->options.bitops.apnd_file = TRUE;
}

void			h_show_only_open_ports(t_job *job)
{
	job->options.bitops.output_only_open_ports = TRUE;
}

int h_file(t_job *job, char *args)
{
    int     r;
    FILE	*fp;
    int     len;
    char *  opt;
    char	buffer[1028];

    if (!args)
    	return (FAILURE);

    if (!(fp = fopen(args, "r")))
        return (FAILURE);
    r = 1;
    //TODO : add support for different files
    while ((r = getline(&buffer, 16, fp)) > 0)
         parse_ip(job->targets, buffer);

    if (line) free(line);
    close(fd);

    if (r < 0)
        return (r);
    return (0);
}
