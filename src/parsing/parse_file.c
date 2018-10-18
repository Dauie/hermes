#include "../../incl/hermes.h"
#include "../../incl/parser.h"


int			parse_ip_file(t_targetset *set, char *filename)
{
	FILE	*fp;
	size_t	len;
	ssize_t read;
	char	*buff;

	len = 1024;
	if (!filename)
		return (hermes_error(FAILURE, "ips file not provided"));
	if (!(buff = memalloc(sizeof(char) * (len + 1))))
		return (hermes_error(FAILURE, "malloc() %s", strerror(errno)));
	if (!(fp = fopen(filename, "r")))
		return (hermes_error(FAILURE, "error opening %s %s", filename, strerror(errno)));
	while ((read = getline(&buff, &len, fp)) > 0)
	{
		if (buff[read - 1] == '\n')
			buff[read - 1] = '\0';
		if (handle_ip(set, buff) == FAILURE)
			return (hermes_error(FAILURE, "issue parsing targets from %s", filename));
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
		return (hermes_error(FAILURE, "worker file not provided"));
	if (!(buff = memalloc(sizeof(char) * (len + 1))))
		return (hermes_error(FAILURE, "malloc() %s", strerror(errno)));
	if (!(fp = fopen(filename, "r")))
		return (hermes_error(FAILURE, "error opening %s %s", filename, strerror(errno)));
	while ((read = getline(&buff, &len, fp)) > 0)
	{
		if (buff[read - 1] == '\n')
			buff[read - 1] = '\0';
		if (parse_worker(set, buff) == FAILURE)
			return (hermes_error(FAILURE, "issue parsing workers from %s", filename));
	}
	free(buff);
	fclose(fp);
	return (SUCCESS);
}

