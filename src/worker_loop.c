# include "../incl/hermes.h"
# include "../incl/message.h"
# include "../incl/binnify.h"

int				handle_obj_offer(t_wrkr *session, uint8_t code, uint8_t *msg, ssize_t msglen)
{
	ssize_t		ret;
	uint16_t	tc;
	uint32_t 	*objlen;
	binn		*obj;

	objlen = (uint32_t*)(msg + MSG_HDRSZ);
	*objlen = ntohl(*objlen);
	if (msglen < OBJ_MSG_HDRSZ || *objlen <= 0 ||
		(code != C_OBJ_OPTS && code != C_OBJ_TARGETS &&
		code != C_OBJ_PS_NRM && code != C_OBJ_PS_ACK &&
		code != C_OBJ_PS_SYN && code !=C_OBJ_PS_UDP))
	{
		tc = msg_tc(T_OBJ_RPLY, C_PARAM_ERR);
		hermes_sendmsgf(session->sock, tc, NULL);
		return (FAILURE);
	}
	if (!(obj = (binn *)malloc(sizeof(uint8_t) * *objlen)))
	{
		tc = msg_tc(T_OBJ_RPLY, C_DENY_OOM);
		hermes_sendmsgf(session->sock, tc, NULL);
		return (FAILURE);
	}
	tc = msg_tc(T_OBJ_RPLY, C_ACCEPT);
	hermes_sendmsgf(session->sock, tc, NULL);
	ret = recv(session->sock, obj, *objlen, MSG_WAITALL);
	if (ret == 0)
	{
		session->stat.running = false;
	}
	if (recv(session->sock, obj, *objlen, MSG_WAITALL) < *objlen)
	{
		free(obj);
		tc = msg_tc(T_OBJ_RPLY, C_RECV_FAIL);
		hermes_sendmsgf(session->sock, tc, NULL);
		return (FAILURE);
	}
	tc = msg_tc(T_OBJ_RPLY, C_RECV_CNFRM);
	hermes_sendmsgf(session->sock, tc, NULL);
	if (code == C_OBJ_OPTS)
		unbinnify_opts(&session->job->opts, obj);
	else if (code == C_OBJ_TARGETS)
		unbinnify_targetset(session->job->targets, obj);
	else if (code == C_OBJ_PS_NRM)
		unbinnify_portset(session->job->ports, obj);
	else if (code == C_OBJ_PS_ACK)
		unbinnify_portset(session->job->ack_ports, obj);
	else if (code == C_OBJ_PS_SYN)
		unbinnify_portset(session->job->syn_ports, obj);
	else
		unbinnify_portset(session->job->udp_ports, obj);
	free(obj);
	return (SUCCESS);
}

int				process_message(t_wrkr *session,
								   uint8_t *msgbuff, ssize_t msglen)
{
	t_msg_hdr	*hdr;

	hdr = (t_msg_hdr*)msgbuff;
	if (hdr->type == T_OBJ)
		handle_obj_offer(session, hdr->code, msgbuff, msglen);
	else
		return (FAILURE);
	return (SUCCESS);
}

int				worker_loop(t_wrkr *session)
{
	ssize_t		ret;
	uint8_t		msgbuff[PKT_SIZE];

	while (session->stat.running == true)
	{
		bzero(msgbuff, PKT_SIZE);
		if ((ret = hermes_recvmsg(session->sock, msgbuff)) < 0)
		{
			hermes_error(FAILURE, 1, "manager disconnected unexpectedly");
			session->stat.running = false;
		}
		else if (ret > 0)
			process_message(session, msgbuff, ret);
	}
	return (SUCCESS);
}
