#include "../incl/libhermese.h"

void		tbldel(char ***tbl)
{
	char	**tmp;

	tmp = *tbl;
	while (tmp) {
		free(tmp);
		tmp = tmp->next;
	}
	free(*tbl);
}

char 		**strsplit(const char *str, char delim)
{

}