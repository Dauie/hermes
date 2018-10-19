#ifndef HERMES_MESSAGE_H
# define HERMES_MESSAGE_H

#ifndef HERMES_H
# include "hermes.h"
#endif

typedef	struct	s_msg_hdr
{
	uint8_t		type;
	uint8_t		code;
	uint16_t	msglen;
}				t_msg_hdr;

typedef struct s_obj_hdr
{
	uint8_t		type;
	uint8_t		code;
	uint16_t	msglen;
	uint32_t	objlen;
}				t_obj_hdr;

typedef union	u_mval
{
	uint8_t		u8;
	uint16_t	u16;
	uint32_t	u32;
	int			i;
	char		*str;
}				t_mval;

/*
**
**
**
**	General Header Layout
**
**    0                   1                   2                   3
**    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**   |     Type      |     Code     |           Msg Length           |
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**   |                             Data                              |
**   +-+-+-+-+-+-+-+-+-+-+-+-+...
**
**
**	Asset Example
**
**    0                   1                   2                   3
**    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**   |     Type      |     Code      |           Msg Length          |
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**   |                           Binn Length ex. (1754)              |
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**
*/
# define MSG_HDRSZ (4)
# define OBJ_MSG_HDRSZ (8)
# define PKT_SIZE (256)


/*
**	Hermes Message Types
*/
# define T_OBJ (0)
# define T_WRK_REQ (1)
# define T_SHUTDOWN (2)

/*
**	T_OBJ Message Codes
*/
# define C_OBJ_OPTS (0)
# define C_OBJ_TARGETS (1)
# define C_OBJ_PS_NRM (2)
# define C_OBJ_PS_ACK (3)
# define C_OBJ_PS_SYN (4)
# define C_OBJ_PS_UDP (5)

/*
**	T_WRK_REQ Codes
*/
# define C_WRK_REQ (0)

/*
**	T_SHUTDOWN Codes
*/
# define C_SHUTDOWN_SFT (0)
# define C_SHUTDOWN_HRD (1)

/*
**	T_START Message Codes
*/
# define C_START (0)
/*
**	Command Codes
*/
# define MC_KILL
# define MC_KILL_NOW



uint16_t	msg_tc(uint8_t type, uint8_t code);
ssize_t		hermes_recvmsg(int sock, uint8_t *msgbuff);
int			hermes_sendmsgf(int sock, uint16_t type_code, char *fmt, ...);
ssize_t		hermes_send_binn(int sock, uint8_t code, binn *obj);

#endif //HERMES_MESSAGE_H
