#ifndef HERMES_MESSAGE_H
# define HERMES_MESSAGE_H

# include "../libhermes/incl/libhermes.h"
# include "../binn/src/binn.h"


typedef union	u_mval
{
	uint8_t		u8;
	uint16_t	u16;
	uint32_t	u32;
	char		*str;
}				t_mval;

typedef	struct s_msg_hdr
{
	uint8_t		type;
	uint8_t		code;
	uint16_t	msglen;
}				t_msg_hdr;

/*
**	Types: Job, Error, Result
 *
** 	Job Codes: JobOffer, JobAccept,
 *
*	Error Codes:
**
**	General header layout
**
**    0                   1                   2                   3
**    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**   |     Type      |     Code     |           Msg Length           |
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Data                              |
**   +-+-+-+-+-+-+-+-+-+-+-+-+...
**
**
**	Job Offer Example
**
**    0                   1                   2                   3
**    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**   |     Type      |     Code      |           Msg Length          |
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**   |                           Binn Length ex. (1754)              |
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**   |                       Version String                          |
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
# define HERMES_MSG_HDRSZ (4)

# define PKT_SIZE (256)


/*
**	Hermes Message Types
*/
# define MT_JOB (0)
# define MT_JOB_REPLY (1)
# define MT_CMD (2)

/*
**	Job Codes
*/
# define MC_JOB_OFFER (0)

/*
**	Reply Codes
*/
# define MC_EMPTY (0)
# define MC_ACCEPT (1)
# define MC_PARAM_ERR (2)
# define MC_DENY_BUSY (3)
# define MC_DENY_OOM (4)
# define MC_RECV_CNFRM (5)
# define MC_RECV_FAIL (6)

/*
**	Command Codes
*/
# define MC_KILL
# define MC_KILL_NOW



uint16_t	type_code(uint8_t type, uint8_t code);
ssize_t		hermes_recv_msg(int sock, uint8_t *msgbuff);
ssize_t		hermes_send_msg(int sock, uint16_t type_code, char *format, ...);
ssize_t		hermes_send_binn(int sock, binn *data);

#endif //HERMES_MESSAGE_H
