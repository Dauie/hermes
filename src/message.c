#include <stdarg.h>
#include "hermes.h"

#define TC (type, code) ()

ssize_t			hermes_send_msg(int sock, uint32_t type_code, char *format, ...)
{
	uint8_t		*p;
	uint8_t		msgbuff[HERMES_MSG_MAX];
	va_list		ap;
	char		*spec;
	t_uints		val;
	ssize_t		ret;

	va_start(ap, format);
	p = msgbuff;
	/* type_code endian needs to be handled */
	memcpy(p, &type_code, sizeof(uint32_t));
	p += sizeof(uint32_t);
	while ((spec = strsep(&format, ",")) && (p - msgbuff) < HERMES_MSG_MAX)
	{
		if (strcmp(spec, "u8") == 0)
		{
			val.u8 = (uint8_t)va_arg(ap, uint32_t);
			memcpy(p, &val.u8, sizeof(uint8_t));
			p += sizeof(uint8_t);
		}
		else if (strcmp(spec, "u16") == 0)
		{
			val.u16 = htons((uint16_t)va_arg(ap, uint32_t));
			memcpy(p, &val.u16, sizeof(uint16_t));
			p += sizeof(uint16_t);
		}
		else if (strcmp(spec, "u32") == 0)
		{
			val.u32 = htonl(va_arg(ap, uint32_t));
			memcpy(p, &val.u32, sizeof(uint32_t));
			p += sizeof(uint32_t);
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
