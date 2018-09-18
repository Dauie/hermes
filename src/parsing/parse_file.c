# include "../../incl/job.h"
# include "../../incl/parser.h"
# include "../../libhermes/incl/bool.h"

int			parse_ip_file(t_targetlist *list, char *args)
{
	FILE	*fp;
	size_t	len;
	char	buffer[1024];

	len = 1024;
	if (!args)
		hermes_error(errno, TRUE, 2, "ip file not provided");
	if (!(fp = fopen(args, "r")))
		hermes_error(errno, TRUE, 2, "fopen()", strerror(errno));
	while ((getline((char **)&buffer, &len, fp)) > 0)
		handle_ip(list, buffer);
	fclose(fp);
	return (SUCCESS);
}


