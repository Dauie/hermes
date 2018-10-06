# include "../incl/message.h"

# include "../incl/hermes.h"


int			worker_loop(t_wsession *session)
{
	uint8_t	buff[HERMES_MSG_MAX] = {0};

	while (session->run)
	{
		hermes_msg_recv();
	}
	return (SUCCESS);
}
