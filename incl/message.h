#ifndef HERMES_MESSAGE_H
#define HERMES_MESSAGE_H
# include "../libhermes/incl/libhermes.h"

typedef union u_uints
{
	uint8_t		u8;
	uint16_t	u16;
	uint32_t	u32;
	char		*str;
}			t_uints;

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
**   |     Type      |     Code     |           Msg Length          |
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
# define HERMES_MSG_MAX (64)

/*
**	Hermes Message Types
*/
# define JOB_MSG (0)
# define ERROR_MSG (1)

/*
**	Job Codes
*/
# define JOB_OFFER (0)
# define JOB_ACCEPT (1)
# define JOB_DENY (2)

uint16_t	type_code(uint8_t type, uint8_t code);
ssize_t		hermes_recv_msg(int sock, uint8_t *msgbuff);
ssize_t		hermes_send_msg(int sock, uint16_t type_code, uint16_t len, char *format, ...);

#endif //HERMES_MESSAGE_H
