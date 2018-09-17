
#include "../incl/libhermese.h"

static size_t	skip(size_t size, const char *s)
{
	while (s[size] == ' ' || s[size] == '\n' ||
		   s[size] == '\t' || s[size] == '\0')
		size--;
	return (size);
}

char		*strtrim(const char *s)
{
	size_t	size;
	char	*ret;

	if (!s)
		return (0);
	while (*s == ' ' || *s == '\n' || *s == '\t' || *s == '\0')
	{
		if (*s == 0)
		{
			if (!(ret = (char*)memalloc(sizeof(char))))
				return (0);
			*ret = 0;
			return (ret);
		}
		s++;
	}
	size = (strlen(s));
	size = skip(size, s);
	size++;
	if (!(ret = (char*)memalloc(sizeof(char) * (size + 1))))
		return (0);
	memcpy(ret, s, size);
	ret[size] = 0;
	return (ret);
}