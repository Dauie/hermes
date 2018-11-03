#include "../incl/hermes.h"
#include "../incl/message.h"

uint16_t		msg_tc(uint8_t type, uint8_t code)
{
	uint16_t tc;

	tc = (uint16_t)code;
	tc = (tc << 8) | type;
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

void 			msg_pack_header(uint8_t *p, uint16_t type_code, uint16_t len)
{
	pack_uint16(&p, type_code);
	pack_uint16(&p, htons(len));
}

size_t			msg_pack_data(uint8_t *msgbuff, char *fmt, va_list ap)
{
	uint8_t		*p;
	char		*spec;
	uint32_t	u32;
	uint16_t	u16;
	uint8_t		u8;
	char		*str;

	p = msgbuff;
	while ((spec = strsep(&fmt, ",")) && (p - msgbuff) < PKT_SIZE)
	{
		if (strcmp(spec, "u8") == 0)
		{
			u8 = (uint8_t)va_arg(ap, uint32_t);
			pack_uint8(&p, u8);
		}
		else if (strcmp(spec, "u16") == 0)
		{
			u16 = htons((uint16_t)va_arg(ap, uint32_t));
			pack_uint16(&p, u16);
		}
		else if (strcmp(spec, "u32") == 0)
		{
			u32 = (uint32_t)va_arg(ap, uint32_t);
			u32 = htonl(u32);
			pack_uint32(&p, u32);
		}
		else if (*fmt == 's')
		{
			str = va_arg(ap, char *);
			pack_string(&p, str);
		}
	}
	return (p - msgbuff);
}

/* TODO: Error handling in this func needs to be improved */
int				hermes_sendmsgf(int sock, uint16_t type_code, char *fmt, ...)
{
	uint8_t		*hp;	/* header pointer */
	uint8_t		*dp;	/* data pointer */
	va_list		ap;		/* vargs list */
	ssize_t		msglen;
	uint8_t		msgbuff[PKT_SIZE] = {0};
	ssize_t		ret;

	msglen = 0;
	hp = msgbuff;
	dp = msgbuff + MSG_HDRSZ;
	if (fmt)
	{
		va_start(ap, fmt);
		msglen += msg_pack_data(dp, fmt, ap);
		va_end(ap);
	}
	msg_pack_header(hp, type_code, (uint16_t)msglen);
	if ((ret = send(sock, msgbuff, (size_t)MSG_HDRSZ + msglen, MSG_WAITALL)) < 0)
		return (hermes_error(FAILURE, "send() %s", strerror(errno)));
	else if (ret != MSG_HDRSZ + msglen)
		return (hermes_error(FAILURE, "hermes_sendmsgf()"));
	return (SUCCESS);
}

ssize_t			hermes_send_binn(int sock, uint8_t code, binn *obj)
{
	uint32_t	objlen;
	ssize_t		ret;
	uint16_t	tc;

	objlen = (uint32_t)binn_size(obj);
	tc = msg_tc(T_OBJ, code);
	hermes_sendmsgf(sock, tc, "u32", objlen);
	if ((ret = send(sock, binn_ptr(obj), objlen, MSG_WAITALL)) < 0)
		return (hermes_error(FAILURE, "send() %s", strerror(errno)));
	if (ret != objlen)
		return (FAILURE);
	return (SUCCESS);
}

/*
**	If hermes_recvmsg returns FAILURE the connection was closed.
**	if Fatal error has occurred, process will be ended.
*/
ssize_t			hermes_recvmsg(int sock, uint8_t *msgbuff)
{
	t_msg_hdr	*hdr;
	ssize_t		hdrlen;
	ssize_t		msglen;

	msglen = 0;
	hdr = (t_msg_hdr*)msgbuff;
	if ((hdrlen = recv(sock, msgbuff, MSG_HDRSZ, MSG_WAITALL)) <= 0 || hdrlen < MSG_HDRSZ)
	{
		if (hdrlen == 0)
			return (ERR_DISCON);
		else if (errno == EWOULDBLOCK || errno == EAGAIN)
			return (0);
		else
			return (hermes_error(FAILURE, "recv() %s", strerror(errno)));
	}
	hdr->msglen = ntohs(hdr->msglen);
	if (hdr->msglen > 0)
	{
		if ((msglen = recv(sock, msgbuff + MSG_HDRSZ, hdr->msglen, MSG_WAITALL)) <= 0)
		{
			if (msglen == 0)
				return (ERR_DISCON);
			else if (errno == EWOULDBLOCK || errno == EAGAIN)
				return (0);
			else
				return (hermes_error(FAILURE, "recv() %s", strerror(errno)));
		}
	}
	return (hdrlen + msglen);
}
