#include "../incl/libhermes.h"

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

size_t		cntwrds(char *str, char c)
{
	size_t	i;
	char	*sc;

	i = 0;
	sc = str;
	if (str[i] != c)
		i++;
	while (*sc)
	{
		if ((*(sc - 1) == c || *(sc - 1) == 0) && *sc != c)
			i++;
		sc++;
	}
	return (i);
}

char				*strsub(char const *s, int start, size_t len)
{
	char	*sub;
	int		i;

	i = 0;
	if (!(sub = (char *)memalloc((sizeof(char) * len) + 1)))
		return (NULL);
	if (s != NULL)
	{
		while (len) {
			sub[i] = s[start];
			i++;
			start++;
			len--;
		}
	}
	return (sub);
}

char 				**strsplit(const char *str, char delim)
{
	int		wcnt;
	char	**ret;
	int		i;
	int		j;
	int		start;

	if (str == NULL || delim == 0)
		return (0);
	wcnt = (int)cntwrds((char *)str, delim);
	if (!(ret = (char**)malloc((sizeof(char*) * (wcnt + 1)))))
		return (0);
	j = -1;
	i = 0;
	while (++j < wcnt)
	{
		while (str[i] && str[i] == delim)
			i++;
		start = i;
		while (str[i] && str[i] != delim)
			i++;
		ret[j] = strsub(str, start, (size_t)i - start);
		i++;
	}
	return (ret);
}