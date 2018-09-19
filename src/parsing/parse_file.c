# include "../../incl/job.h"
# include "../../incl/parser.h"

int			parse_ip_file(t_targetlist *list, char *filename)
{
	FILE	*fp;
	size_t	len;
	ssize_t read;
	char	*buff;

	len = 1024;
	if (!filename)
		hermes_error(errno, TRUE, 2, "ip file not provided");
	if (!(buff = memalloc(sizeof(char) * (len + 1))))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	if (!(fp = fopen(filename, "r")))
		hermes_error(errno, TRUE, 3, "error opening", filename, strerror(errno));
	while ((read = getline(&buff, &len, fp)) > 0)
	{
		if (buff[read - 1] == '\n')
			buff[read - 1] = '\0';
		if (handle_ip(list, buff) == FAILURE)
			hermes_error(INPUT_ERROR, TRUE, 2, "issue parsing targets from", filename);
	}
	free(buff);
	fclose(fp);
	return (SUCCESS);
}

int			parse_worker_file(t_workerlist *list, char *filename)
{
	FILE	*fp;
	size_t	len;
	ssize_t read;
	char	*buff;

	len = 1024;
	if (!filename)
		hermes_error(errno, TRUE, 2, "worker file not provided");
	if (!(buff = memalloc(sizeof(char) * (len + 1))))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	if (!(fp = fopen(filename, "r")))
		hermes_error(errno, TRUE, 3, "error opening", filename, strerror(errno));
	while ((read = getline(&buff, &len, fp)) > 0)
	{
		if (buff[read - 1] == '\n')
			buff[read - 1] = '\0';
		if (parse_worker(list, buff) == FAILURE)
			hermes_error(INPUT_ERROR, TRUE, 2, "issue parsing targets from", filename);
	}
	free(buff);
	fclose(fp);
	return (SUCCESS);
}

