
# include "../incl/libhermes.h"

int				hermes_error(int errcode, char *fmt, ...) {
	va_list ap;
	char buff[512] = {0};

	if (fmt)
	{
		va_start(ap, fmt);
		vsprintf(buff, fmt, ap);
		dprintf(STDERR_FILENO, "hermes: Error - %s.\n", buff);
		va_end(ap);
	}
	if (errcode == EXIT_FAILURE)
		exit(errcode);
	else
		return (errcode);
}
