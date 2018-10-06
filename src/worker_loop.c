# include "../incl/hermes.h"

int				handle_job_offer_request(t_wsession *session,
											uint8_t *msgbuff)
{
	if (session->stat.working == TRUE)
	{
		hermes_send_msg(session->sock, JOB_OFFER_RPLY, JOB_DENY)
	}
}

int				process_request(t_wsession *session, uint8_t *msgbuff)
{
	t_msg_hdr	*hdr;

	hdr = (t_msg_hdr*)msgbuff;
	if (hdr->type == JOB_OFFER_REQ)
		handle_job_offer_request(session, msgbuff);
		/* TODO parse job message and dispatch */;
	else if (hdr->type == ERROR_MSG)
		/*TODO parse error message and dispatch */;

	return (SUCCESS);
}

int			worker_loop(t_wsession *session)
{
	ssize_t	ret;
	uint8_t	msgbuff[HERMES_MSG_MAX] = {0};

	while (session->stat.running == TRUE)
	{
		if ((ret = hermes_recv_msg(session->sock, msgbuff)) < 0)
		{
			printf("manager disconnected\n");
			session->stat.running = FALSE;
		}
		else if (ret > 0)
			process_request(session, msgbuff);
	}
	return (SUCCESS);
}
