#include "hermes.h"

uint16_t	type_code(uint8_t type, uint8_t code)
{
	uint16_t tc;

	tc = (uint16_t)type;
	tc = (tc << 8) | code;
	return (tc);
}

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

void			pack_string(uint8_t **p, char *str)
{
	size_t		len;

	len = strlen(str);
	memcpy(*p, str, len);
	*p += len;
}

ssize_t			hermes_send_binn(int sock, binn *data)
{
	int			len;
	ssize_t		ret;
	void		*binn;

	len = binn_size(data);
	binn = binn_ptr(data);
	ret = send(sock, &binn, len, 0);
	return (ret);
}

void 			msg_pack_header(uint8_t *p, uint16_t type_code, uint16_t len)
{
	pack_uint16(&p, type_code);
	pack_uint16(&p, len);
}

size_t			msg_pack_data(uint8_t *msgbuff, va_list *ap, char *frmt)
{
	uint8_t		*p;
	char		*spec;
	t_mval		val;

	p = msgbuff;
	while ((spec = strsep(&frmt, ",")) && (p - msgbuff) < PKT_SIZE)
	{
		if (strcmp(spec, "u8") == 0)
		{
			val.u8 = (uint8_t)va_arg(*ap, uint32_t);
			pack_uint8(&p, val.u8);
		}
		else if (strcmp(spec, "u16") == 0)
		{
			val.u16 = htons((uint16_t)va_arg(*ap, uint32_t));
			pack_uint16(&p, val.u16);
		}
		else if (strcmp(spec, "u32") == 0)
		{
			val.u32 = htonl(va_arg(*ap, uint32_t));
			pack_uint32(&p, val.u32);
		}
		else if (*frmt == 's')
		{
			val.str = va_arg(*ap, char *);
			pack_string(&p, val.str);
		}
	}
	return (p - msgbuff);
}

/* TODO: Have better bounds checks while packing to prevent going over MAX_MSG */
ssize_t			hermes_send_msg(int sock, uint16_t type_code, char *format, ...)
{
	uint8_t		*hp;
	uint8_t		*dp;
	va_list		ap;
	size_t		datalen;
	uint8_t		msgbuff[PKT_SIZE] = {0};
	ssize_t		ret;

	datalen = 0;
	hp = msgbuff;
	dp = msgbuff + HERMES_MSG_HDRSZ;
	if (format)
	{
		va_start(ap, format);
		datalen = msg_pack_data(dp, &ap, format);
		va_end(ap);
	}
	msg_pack_header(hp, type_code, (uint16_t) datalen);
	if ((ret = send(sock, msgbuff, HERMES_MSG_HDRSZ + datalen, 0)) < 0)
		hermes_error(errno, TRUE, 2, "send()", strerror(errno));
	return (ret);
}

/*
**	If hermes_recv_msg returns -1 the connection was closed.
*/
ssize_t			hermes_recv_msg(int sock, uint8_t *msgbuff)
{
	t_msg_hdr	*hdr;
	ssize_t		ret;

	hdr = (t_msg_hdr*)msgbuff;
	if ((ret = recv(sock, msgbuff, HERMES_MSG_HDRSZ, MSG_WAITALL)) <= 0)
	{
		if (ret == 0)
			return (-1);
		else if (errno == EWOULDBLOCK || errno == EAGAIN)
			return (0);
		else
			hermes_error(errno, TRUE, 2, "recv()", strerror(errno));
	}
	hdr->msglen = ntohs(hdr->msglen);
	if (hdr->msglen > 0)
	{
		if ((ret = recv(sock, msgbuff + HERMES_MSG_HDRSZ, hdr->msglen, MSG_WAITALL)) <= 0)
		{
			if (ret == 0)
				return (-1);
			else if (errno == EWOULDBLOCK || errno == EAGAIN)
				return (0);
			else
				hermes_error(errno, TRUE, 2, "recv()", strerror(errno));
		}
	}
	return (ret);
}
