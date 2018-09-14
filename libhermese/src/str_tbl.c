#include "../incl/libhermese.h"

void		tbldel(char ***tbl)
{
	char	**tmp;

	tmp = *tbl;
	while (tmp) {
		free(tmp);
		tmp++;
	}
	free(*tbl);
}

char 		**strsplit(const char *str, char delim)
{
	/* TODO implement a strsplit func */
}