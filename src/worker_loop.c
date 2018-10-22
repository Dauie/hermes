# include "../incl/hermes.h"
# include "../incl/message.h"
# include "../incl/binnify.h"

int					handle_obj_offer(t_wrkr *session, uint8_t code, uint8_t *msg)
{
	ssize_t			ret;
	binn			*obj;
	t_obj_hdr		*hdr;

	printf("handling obj offer\n");
	hdr = (t_obj_hdr *)msg;
	hdr->objlen = ntohl(hdr->objlen);
	printf("%i %i %i %u\n", hdr->type, hdr->code, hdr->msglen, hdr->objlen);
	if (hdr->objlen <= 0)
		return (FAILURE);
	if (!(obj = (binn *)malloc(sizeof(uint8_t) * hdr->objlen)))
		return (FAILURE);
	ret = recv(session->sock, obj, hdr->objlen, MSG_WAITALL);
	if (ret == 0)
	{
		printf("failed to recv obj\n");
		free(obj);
		session->stat.running = false;
		return (FAILURE);
	}
	printf("recved obj %zu\n", ret);
	if (session->job == NULL)
		if (!(session->job = new_job()))
			return (hermes_error(FAILURE, "malloc() %s", strerror(errno)));
	if (code == C_OBJ_OPTS)
	{
		unbinnify_opts(session->job->opts, obj);
		printf("received options\n");
	}
	else if (code == C_OBJ_TARGETS)
	{
		if (!session->job->targets)
			if (!(session->job->targets = new_targetset()))
				return (hermes_error(FAILURE, "malloc() %s", strerror(errno)));
		unbinnify_targetset(session->job->targets, obj);
		session->stat.work_requested = false;
		printf("received targetset\n");
	}
	else if (code == C_OBJ_PS_NRM)
	{
		if (!session->job->ports)
			if (!(session->job->ports = new_portset()))
				return (hermes_error(FAILURE, "malloc() %s", strerror(errno)));
		unbinnify_portset(session->job->ports, obj);
		session->stat.initilized = true;
		printf("received scan portset\n");
	}
	else if (code == C_OBJ_PS_ACK)
	{
		if (!(session->job->ack_ports))
			if (!(session->job->ack_ports = new_portset()))
				return (hermes_error(FAILURE, "malloc() %s", strerror(errno)));
		unbinnify_portset(session->job->ack_ports, obj);
		printf("received ack portset\n");
	}
	else if (code == C_OBJ_PS_SYN)
	{
		if (!session->job->syn_ports)
			if (!(session->job->syn_ports = new_portset()))
				return (hermes_error(FAILURE, "malloc() %s", strerror(errno)));
		unbinnify_portset(session->job->syn_ports, obj);
		printf("received syn portset\n");
	}
	else if (code == C_OBJ_PS_UDP)
	{
		if (!session->job->udp_ports)
			if (!(session->job->udp_ports = new_portset()))
				return (hermes_error(FAILURE, "malloc() %s", strerror(errno)));
		unbinnify_portset(session->job->udp_ports, obj);
		printf("received udp portset\n");
	}
	free(obj);
	return (SUCCESS);
}

int					process_message(t_wrkr *session, uint8_t *msgbuff)
{
	t_msg_hdr		*hdr;

	printf("processing message\n");
	hdr = (t_msg_hdr*)msgbuff;
	printf("type: %i code: %i\n", hdr->type, hdr->code);
	if (hdr->type == T_OBJ)
		handle_obj_offer(session, hdr->code, msgbuff);
	else if (hdr->type == T_SHUTDOWN)
	{
		if (hdr->code == C_SHUTDOWN_SFT)
			session->stat.running = false;
		else if (hdr->code == C_SHUTDOWN_HRD)
			hermes_error(EXIT_FAILURE, "Manager signaled emergency shutdown.");
		else
			return (FAILURE);
	}
	else
		return (FAILURE);
	return (SUCCESS);
}

int					worker_loop(t_wrkr *session)
{
	ssize_t			rc;
	uint8_t			msgbuff[PKT_SIZE];
	int				timeout;
	struct pollfd	fds[1];

	memset(&fds, 0, sizeof(fds));
	fds[0].fd = session->sock;
	fds[0].events = POLLIN;
	timeout = (500);	/* 1/2 sec */
	printf("worker loop start\n");
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
		else if (rc == 0)
		{
			printf("timeout\n");
		}
		else if (rc > 0 && fds[0].revents & POLLIN)
		{
			if ((rc = hermes_recvmsg(session->sock, msgbuff)) < 0)
			{
				session->stat.running = false;
				hermes_error(FAILURE, "manager disconnected unexpectedly");
			}
			else if (rc > 0)
			{
				process_message(session, msgbuff);
				bzero(msgbuff, PKT_SIZE);
			}
		}
		if (session->stat.initilized && (!session->job->targets || session->job->targets->total == 0)
			&& !session->stat.work_requested)
		{
			printf("sending work request\n");
			hermes_sendmsgf(session->sock, msg_tc(T_WRK_REQ, C_WRK_REQ), NULL);
			session->stat.work_requested = true;
			printf("work request sent\n");
		}
	}
	return (SUCCESS);
}
