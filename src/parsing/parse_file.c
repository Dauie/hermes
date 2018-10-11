# include "../../incl/job.h"
# include "../../incl/parser.h"

int			parse_ip_file(t_targetset *set, char *filename)
{
	FILE	*fp;
	size_t	len;
	ssize_t read;
	char	*buff;

	len = 1024;
	if (!filename)
		return (hermes_error(FAILURE, 2, "ips file not provided"));
	if (!(buff = memalloc(sizeof(char) * (len + 1))))
		return (hermes_error(FAILURE, 2, "malloc()", strerror(errno)));
	if (!(fp = fopen(filename, "r")))
		return (hermes_error(FAILURE, 3, "error opening", filename, strerror(errno)));
	while ((read = getline(&buff, &len, fp)) > 0)
	{
		if (buff[read - 1] == '\n')
			buff[read - 1] = '\0';
		if (handle_ip(set, buff) == FAILURE)
			return (hermes_error(FAILURE, 2, "issue parsing targets from", filename));
	}
	free(buff);
	fclose(fp);
	return (SUCCESS);
}

int			parse_worker_file(t_workerset *set, char *filename)
{
	FILE	*fp;
	size_t	len;
	ssize_t read;
	char	*buff;

	len = 1024;
	if (!filename)
		return (hermes_error(FAILURE, 2, "worker file not provided"));
	if (!(buff = memalloc(sizeof(char) * (len + 1))))
		return (hermes_error(FAILURE, 2, "malloc()", strerror(errno)));
	if (!(fp = fopen(filename, "r")))
		return (hermes_error(FAILURE, 3, "error opening", filename, strerror(errno)));
	while ((read = getline(&buff, &len, fp)) > 0)
	{
		if (buff[read - 1] == '\n')
			buff[read - 1] = '\0';
		if (parse_worker(set, buff) == FAILURE)
			return (hermes_error(FAILURE, 2, "issue parsing targets from", filename));
	}
	free(buff);
	fclose(fp);
	return (SUCCESS);
}

