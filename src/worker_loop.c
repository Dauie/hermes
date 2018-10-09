# include "../incl/hermes.h"

int				handle_job_offer_request(t_wsession *session, uint8_t *msgbuff, ssize_t msglen)
{
	uint16_t	tc;
	uint32_t 	*len;
	binn		*obj;

	if (msglen < (ssize_t)(HERMES_MSG_HDRSZ + sizeof(uint32_t)))
	{
		tc = type_code(MT_JOB_REPLY, MC_JOB_PARAM_ERR);
		hermes_send_msg(session->sock, tc, NULL);
		return (FAILURE);
	}
	if (session->stat.working == TRUE)
	{
		tc = type_code(MT_JOB_REPLY, MC_JOB_DENY_BUSY);
		hermes_send_msg(session->sock, tc, NULL);
		return (FAILURE);
	}
	len = (uint32_t*)(msgbuff + HERMES_MSG_HDRSZ);
	*len = ntohl(*len);
	if (*len <= 0)
	{
		tc = type_code(MT_JOB_REPLY, MC_JOB_PARAM_ERR);
		hermes_send_msg(session->sock, tc, NULL);
		return (FAILURE);
	}
	if (!(obj = (binn *)malloc(sizeof(uint8_t) * *len)))
	{
		tc = type_code(MT_JOB_REPLY, MC_JOB_DENY_OOM);
		hermes_send_msg(session->sock, tc, NULL);
		return (FAILURE);
	}
	tc = type_code(MT_JOB_REPLY, MC_JOB_ACCEPT);
	hermes_send_msg(session->sock, tc, NULL);
	if (recv(session->sock, obj, *len, MSG_WAITALL) < *len)
	{
		tc = type_code(MT_JOB_REPLY, MC_JOB_RECV_FAIL);
		hermes_send_msg(session->sock, tc, NULL);
		return (FAILURE);
	}
	tc = type_code(MT_JOB_REPLY, MC_JOB_RECV_CNFRM);
	hermes_send_msg(session->sock, tc, NULL);
	unbinnify_job(obj, &session->job);
	/*TODO split work, make threads */
	return (SUCCESS);
}

int				process_request(t_wsession *session, uint8_t *msgbuff, ssize_t msglen)
{
	t_msg_hdr	*hdr;

	hdr = (t_msg_hdr*)msgbuff;
	if (hdr->type == MT_JOB)
		handle_job_offer_request(session, msgbuff, msglen);
	return (SUCCESS);
}

int			worker_loop(t_wsession *session)
{
	ssize_t	ret;
	uint8_t	msgbuff[PKT_SIZE] = {0};

	while (session->stat.running == TRUE)
	{
		if ((ret = hermes_recv_msg(session->sock, msgbuff)) < 0)
		{
			printf("manager disconnected\n");
			session->stat.running = FALSE;
		}
		else if (ret > 0)
			process_request(session, msgbuff, ret);
	}
	return (SUCCESS);
}
