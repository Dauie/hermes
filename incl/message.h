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
# define HERMES_MSG_MAX (64)

# define HERMES_PACK_SIZE (512)


/*
**	Hermes Message Types
*/
# define MT_JOB
# define MT_JOB_REPLY

# define MT_CMD

/*
**	Job Request Codes
*/
# define MC_JOB_OFFER

/*
**	Job Reply Codes
*/
# define MC_JOB_ACCEPT
# define MC_JOB_DENY

/*
**	Command Codes
*/
# define MC_KILL
# define MC_KILL_NOW



uint16_t	type_code(uint8_t type, uint8_t code);
ssize_t		hermes_recv_msg(int sock, uint8_t *msgbuff);
ssize_t		hermes_send_msg(int sock, uint16_t type_code, char *format, ...);
ssize_t		hermes_send_binn(int sock, uint16_t type_code, binn *data);

#endif //HERMES_MESSAGE_H
