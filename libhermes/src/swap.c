#include "../incl/libhermes.h"

void			swap_uint8(uint8_t *l, uint8_t *r)
{
	uint8_t		tmp;

	tmp = *l;
	*l = *r;
	*r = tmp;
}

void			swap_uint16(uint16_t *l, uint16_t *r)
{
	uint16_t	tmp;

	tmp = *l;
	*l = *r;
	*r = tmp;
}

void			swap_uint32(uint32_t *l, uint32_t *r)
{
	uint32_t	tmp;

	tmp = *l;
	*l = *r;
	*r = tmp;
}

void			swap_uint64(uint64_t *l, uint64_t *r)
{
	uint64_t	tmp;

	tmp = *l;
	*l = *r;
	*r = tmp;
}
