#ifndef HERMES_MESSAGE_H
#define HERMES_MESSAGE_H

#include "hermes.h"

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
**   |               Type            |              Code             |
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**   |                             Data
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+...
**
**
**	Job Offer Example
**
**    0                   1                   2                   3
**    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**   |               Type   (0)     |              Code   (0)        |
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**   |                             Length  ex. (1754)                |
**   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

# define HERMES_MSG_MAX (64)

/*
**	Hermes Message Types
*/
# define MSGTYPE_JOB	(0)
# define MSGTYPE_RES	(1)
# define MSGTYPE_CMD	(2)
# define MSGTYPE_		(3)

/*
**	Job Codes
*/
# define JOB_OFFER		(0)
# define JOB_ACCEPT 	(1)
# define JOB_DENY		(2)
# define JOB_DELIVERY	(3)

ssize_t		hermes_send_msg(int sock, uint8_t type, uint8_t code, void *data, char *type_fmt);
void		hermes_recv_msg(int sock);

#endif //HERMES_MESSAGE_H
