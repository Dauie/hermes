#include "../incl/hermes.h"
#include "../incl/message.h"

uint32_t	type_code(uint8_t type, uint8_t code)
{
	uint32_t 	type_code;

	memcpy(&type_code, &type, sizeof(uint8_t));
	type_code <<= 16;
	memcpy(&type_code, &code, sizeof(uint8_t));
	return (type_code);
}

void 	msg_load_data(uint8_t **p, va_list ap, char type)
{
	binn	obj;

	if (type == 'b')
	{
		obj = va_arg(ap, binn);
		memcpy(p, binn_ptr(&obj), binn_size(&obj));
		return ;
	}
	hermes_error(FORMAT_ERROR, TRUE, -1, "wrong msg data format");
}

int		hermes_msg(uint8_t buffer[], uint32_t type_code, char *format, ...)
{
	uint8_t		*p;
	va_list 	ap;
	char		*next;

	p = buffer;
	memcpy(p, &type_code, sizeof(uint32_t));
	p += sizeof(uint32_t);
	va_start(ap, format);
	while (*format)
	{
		if ((next = strchr(format, '%')))
		{
			next++;
			if (*next)
				msg_load_data(&p, ap, *next);
		}
		else
		{
			;// TODO : do something
		}
		format = next;
	}
	va_end(ap);
	// TODO : fmt args
	//memcpy(p, &, sizeof(uint16_t));
}

ssize_t		hermes_send_msg(int sock, uint8_t type, uint8_t code, void *data, char *type_fmt)
{
	uint8_t		msg_buff[HERMES_MSG_MAX];
												/*TODO*/
	hermes_msg(msg_buff, type_code(type, code), type_fmt, data);
	return (send(sock, msg_buff, sizeof(msg_buff), 0));
}

void		hermes_recv_msg(int sock /*TODO : function handler pointer?*/)
{
	uint8_t		msg_buff[HERMES_MSG_MAX];

	if (recv(sock, msg_buff, sizeof(msg_buff), 0) < 0)
		;
}
