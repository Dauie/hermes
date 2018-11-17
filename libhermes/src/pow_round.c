# include "../incl/libhermes.h"

unsigned int			pow2_round(unsigned int nb)
{
	nb--;
	nb |= nb >> 1;
	nb |= nb >> 2;
	nb |= nb >> 4;
	nb |= nb >> 8;
	nb |= nb >> 16;
	nb++;
	return (nb);
}