#ifndef HERMES_MESSAGE_H
#define HERMES_MESSAGE_H

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
# define MSGTYPE_JOB (0)

/*
**	Job Codes
*/
# define JOB_OFFER (0)
# define JOB_ACCEPT (1)
# define JOB_DENY (2)
# define

#endif //HERMES_MESSAGE_H
