
# include "../incl/libhermes.h"

int				hermes_error(int errcode, char *fmt, ...) {
	va_list ap;
	char buff[512] = {0};

	if (fmt)
	{
		va_start(ap, fmt);
		sprintf(buff, fmt, ap);
		va_end(ap);
		dprintf(STDERR_FILENO, "hermese: Error - %s.\n", buff);
	}
	if (errcode == EXIT_FAILURE)
		exit(errcode);
	else
		return (errcode);
}
