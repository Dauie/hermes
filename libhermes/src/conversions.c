#include "../incl/libhermes.h"

static char		*get_num(char *num, int n, int base)
{
    *--num = '0' + (n % base);
    n /= base;
    while (n != 0)
    {
        *--num = '0' + (n % base);
        n /= base;
    }
    return (num);
}

char			*itoa_base(int n, int base)
{
    char	*num;

    if (!(num = (char*)memalloc(10)))
        return (0);
    if (n >= 0 && num)
        num = get_num(num, n, base);
    else if (num)
    {
        *--num = '0' - (n % base);
        n /= base;
        while (n != 0)
        {
            *--num = '0' - (n % base);
            n /= base;
        }
        *--num = '-';
    }
    return (strdup(num));
}

char            *itoa(int n)
{
    return (itoa_base(n, 10));
}