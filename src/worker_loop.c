# include "../incl/hermes.h"

int				handle_job_offer(uint8_t *msgbuff)
{

}

int				process_request(uint8_t *msgbuff)
{
	t_msg_hdr	*hdr;

	hdr = (t_msg_hdr*)msgbuff;
	if (hdr->type == JOB_MSG)
	{
		if (hdr->code == JOB_OFFER)
		{

		}
	}
		/* TODO parse job message and dispatch */;
	else if (hdr->type == ERROR_MSG)
		/*TODO parse error message and dispatch */;

	return (SUCCESS);
}

int			worker_loop(t_wsession *session)
{
	ssize_t	ret;
	uint8_t	msgbuff[HERMES_MSG_MAX] = {0};

	while (session->run)
	{
		if ((ret = hermes_recv_msg(session->sock, msgbuff)) < 0)
		{
			printf("manager disconnected\n");
			session->run = FALSE;
		}
		else if (ret > 0)
			process_request(msgbuff);
	}
	return (SUCCESS);
}
