
#define TC (type, code) ()

uint32_t	type_code(uint16_t type, uint16_t code)
{
	uint32_t 	type_code;

	memcpy(&type_code, &type, sizeof(uint16_t));
	type_code <<= 16;
	memcpy(&type_code, code, sizeof(uint16_t));
	return (type_code);
}

int		hermes_msg(uint8_t **buffer, uint32_t type_code, char *format, ...)
{
	uint8_t		*p;

	p = msg;
	memcpy(p, &type_code, sizeof(uint32_t));
	p += sizeof(uint32_t);
	memcpy(p, &, sizeof(uint16_t));
}

void 	hermes_send_msg(int sock, uint16_t type, uint16_t code)
{
	uint8_t		msg_buff[HERMES_MSG_MAX];

	hermes_msg(msg_buff, type_code(type, code), "");
	if (send(sock, msg_buff, sizeof(uint8_t * HERMES_MSG_MAX), 0) < 0)
		;
}

void	hermes_recv_msg(int sock)
{
	uint8_t		msg_buff[HERMES_MSG_MAX];

	if (recv(sock, msg_buff, sizeof(uint8_t * HERMES_MSG_MAX), 0) < 0)
		;

}
