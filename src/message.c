#include <stdarg.h>
#include "hermes.h"

#define TC (type, code) ()

void			pack_uint8(uint8_t **p, uint8_t val)
{
	memcpy(*p, &val, sizeof(uint8_t));
	*p += sizeof(uint8_t);
}

void			pack_uint16(uint8_t **p, uint16_t val)
{
	memcpy(*p, &val, sizeof(uint16_t));
	*p += sizeof(uint16_t);
}

void			pack_uint32(uint8_t **p, uint32_t val)
{
	memcpy(*p, &val, sizeof(uint32_t));
	*p += sizeof(uint32_t);
}


ssize_t			hermes_send_msg(int sock, uint16_t type_code, int len, char *format, ...)
{
	uint8_t		*p;
	uint8_t		msgbuff[HERMES_MSG_MAX];
	va_list		ap;
	char		*spec;
	t_uints		val;
	ssize_t		ret;

	va_start(ap, format);
	p = msgbuff;
	pack_uint16(&p, type_code);
	pack_uint16(&p, type_code);
	while ((spec = strsep(&format, ",")) && (p - msgbuff) < HERMES_MSG_MAX)
	{
		if (strcmp(spec, "u8") == 0)
		{
			val.u8 = (uint8_t)va_arg(ap, uint32_t);
			pack_uint8(&p, val.u8);
		}
		else if (strcmp(spec, "u16") == 0)
		{
			val.u16 = htons((uint16_t)va_arg(ap, uint32_t));
			pack_uint16(&p, val.u16);
		}
		else if (strcmp(spec, "u32") == 0)
		{
			val.u32 = htonl(va_arg(ap, uint32_t));
			pack_uint32(&p, val.u32);
		}
		else if (*format == 's')
		{
			val.str = va_arg(ap, char *);
			memcpy(p, val.str, strlen(val.str));
		}
	}
	if ((ret = send(sock, msgbuff, msgbuff - p, 0)) < 0)
		hermes_error(errno, TRUE, 2, "send()", strerror(errno));
	va_end(ap);
	return (ret);
}

/*
**	If hermes_recv_msg returns -1 the connection was closed.
*/
ssize_t		hermes_recv_msg(int sock, uint8_t *msgbuff)
{
	ssize_t	ret;

	if ((ret = recv(sock, msgbuff, HERMES_MSG_MAX, MSG_DONTWAIT)) <= 0)
	{
		if (ret == 0)
			return (-1);
		else if (errno == EWOULDBLOCK || errno == EAGAIN)
			return (0);
		else
			hermes_error(errno, TRUE, 2, "recv()", strerror(errno));
	}
	return (ret);
}
