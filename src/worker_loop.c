# include "../incl/hermes.h"


int			worker_loop(t_wsession *session)
{
	ssize_t	ret;
	uint8_t	buff[HERMES_MSG_MAX] = {0};

	while (session->run)
	{
		if ((ret = hermes_recv_msg(session->sock, buff)) < 0)
		{
			printf("manager disconnected");
			session->run = FALSE;
		}
		else if (ret > 0)
		{
			/* Process request */
		}
	}
	return (SUCCESS);
}
