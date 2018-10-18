# include "../incl/hermes.h"
# include "../incl/message.h"
# include "../incl/binnify.h"

int					handle_obj_offer(t_wrkr *session, uint8_t code, uint8_t *msg)
{
	ssize_t			ret;
	uint32_t 		*objlen;
	binn			*obj;

	objlen = (uint32_t*)(msg + MSG_HDRSZ);
	*objlen = ntohl(*objlen);
	if (*objlen <= 0)
		return (FAILURE);
	if (!(obj = (binn *)malloc(sizeof(uint8_t) * *objlen)))
		return (FAILURE);
	ret = recv(session->sock, obj, *objlen, MSG_WAITALL);
	if (ret == 0)
	{
		free(obj);
		session->stat.running = false;
		return (FAILURE);
	}
	if (code == C_OBJ_OPTS) {
		unbinnify_opts(&session->job->opts, obj);
		printf("received options");
	}
	else if (code == C_OBJ_TARGETS) {
		unbinnify_targetset(session->job->targets, obj);
		printf("received targetset\n");
	}
	else if (code == C_OBJ_PS_NRM) {
		unbinnify_portset(session->job->ports, obj);
		printf("received scan portset\n");
	}
	else if (code == C_OBJ_PS_ACK) {
		unbinnify_portset(session->job->ack_ports, obj);
		printf("received ack portset\n");
	}
	else if (code == C_OBJ_PS_SYN) {
		unbinnify_portset(session->job->syn_ports, obj);
		printf("received syn portset\n");
	}
	else if (code == C_OBJ_PS_UDP) {
		unbinnify_portset(session->job->udp_ports, obj);
		printf("received udp portset\n");
	}
	free(obj);
	return (SUCCESS);
}

int					process_message(t_wrkr *session, uint8_t *msgbuff, ssize_t recvsz)
{
	t_msg_hdr		*hdr;

	hdr = (t_msg_hdr*)msgbuff;
	if (hdr->type == T_OBJ && recvsz >= OBJ_MSG_HDRSZ)
		handle_obj_offer(session, hdr->code, msgbuff);
	else
		return (FAILURE);
	return (SUCCESS);
}

//void 			run_hermes()
//{
//}

int					worker_loop(t_wrkr *session)
{
	ssize_t			rc;
	uint8_t			msgbuff[PKT_SIZE];
	int				timeout;
	struct pollfd	fds[1] = {0};

	fds[0].fd = session->sock;
	fds[0].events = POLLIN;
	timeout = (500);	/* 1/2 sec */
	while (session->stat.running == true)
	{
		rc = poll(fds, 1, timeout);
		if (rc < 0)
		{
			if (errno != EAGAIN && errno != EINTR)
			{
				session->stat.running = false;
				hermes_error(FAILURE, "poll()", strerror(errno));
			}
		}
		else if (rc > 0)
		{
			if ((rc = hermes_recvmsg(session->sock, msgbuff)) < 0)
			{
				session->stat.running = false;
				hermes_error(FAILURE, "manager disconnected unexpectedly");
			}
			else if (rc > 0)
			{
				process_message(session, msgbuff, rc);
				bzero(msgbuff, PKT_SIZE);
			}
		}
		/* Do work with threads */
	}
	return (SUCCESS);
}
