#include <zconf.h>
# include "../incl/libhermes.h"

int				hermes_error(int errcode, int fatal, int str_amt, ...)
{
	va_list		ap;
	size_t 		i;
	char		buff[2048] = {0};
	int			strcnt;

	i = 0;
	strcnt = -1;
	if (str_amt > 0)
	{
		va_start(ap, str_amt);
		while (++strcnt < str_amt)
			i += sprintf(buff + i, " %s", va_arg(ap,
					char *));
		va_end(ap);
		dprintf(STDERR_FILENO, "hermese: Error - %s.", buff);
	}
	if (fatal == TRUE)
		exit(errcode);
	else
		return (errcode);
}
