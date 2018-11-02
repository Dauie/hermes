# include "../incl/hermes.h"
# include "../incl/message.h"
# include "../incl/binnify.h"

void                sig_kill(int sig)
{
	signal(sig, SIG_IGN);
	hermes_error(EXIT_FAILURE, "kill signal sent -- goodbye\n");
}

int					handle_obj_offer(t_wmgr *session, uint8_t code, uint8_t *msg)
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
		printf("failed to recv obj, mgr disconnected early\n");
		free(obj);
		session->stat.running = false;
		return (FAILURE);
	}
	printf("recved obj %zu\n", ret);
	if (code == C_OBJ_ENV)
	{
		unbinnify_env(&session->env, obj);
		if (!(session->tpool = init_threadpool(&session->env,
		                                       &session->targets, &session->results)))
			hermes_error(EXIT_FAILURE, "init_threadpool() failure");
		session->stat.initilized = true;
		printf("worker initialized\n");
	}
	else if (code == C_OBJ_TARGETS)
	{
		tpool_event(session->tpool);
		pthread_mutex_lock(&session->tpool->work_pool_mtx);
		unbinnify_targetset(&session->targets, obj);
		pthread_mutex_unlock(&session->tpool->work_pool_mtx);
		session->stat.work_requested = false;
		session->stat.has_work = true;
		printf("received targetset\n");
	}
	free(obj);
	return (SUCCESS);
}

/* int              initialize(t_wmgr **session, uint8_t n_threads)
 * {
 *      if (new_tpool(session->tpool) == false)
 *          return (FAILURE);
 *      return (SUCCESS);
 * }
 */

int					process_message(t_wmgr *session, uint8_t *msgbuff)
{
	t_msg_hdr		*hdr;

	/* TODO :
	 * if initializer is sent:
	 *      session = initialize(t_wmgr &session, uint8_t n_threads);
	 */
	printf("processing message\n");
	hdr = (t_msg_hdr*)msgbuff;
	printf("type: %i code: %i\n", hdr->type, hdr->code);
	if (hdr->type == T_OBJ)
		handle_obj_offer(session, hdr->code, msgbuff);
	else if (hdr->type == T_SHUTDOWN)
	{
		printf("Manager sent shutdown signal\n");
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

int					send_results(t_wmgr *session)
{
	binn			*obj;

	printf("sending results\n");
	if (!session->results.results)
		return (FAILURE);
	obj = binnify_resultset(&session->results);
	if (hermes_send_binn(session->sock, C_OBJ_RES, obj) < 0)
	{
		free(obj);
		return (hermes_error(FAILURE, "send_results()"));
	}
	free(obj);
	return (SUCCESS);
}

void				poll_mgr_messages(t_wmgr *session, struct pollfd *fds)
{
	ssize_t			ret;
	uint8_t			msgbuff[PKT_SIZE];

	bzero(msgbuff, PKT_SIZE);
	ret = poll(fds, 1, WKRMGR_POLL_TIMEO);
	if (ret < 0)
	{
		if (errno != EAGAIN && errno != EINTR)
		{
			session->stat.running = false;
			hermes_error(FAILURE, "poll()", strerror(errno));
		}
	}
	else if (ret == 0)
		printf("timeout\n");
	else if (ret > 0 && fds[0].revents & POLLIN)
	{
		if ((ret = hermes_recvmsg(session->sock, msgbuff)) < 0)
		{
			session->stat.running = false;
			hermes_error(FAILURE, "manager disconnected unexpectedly");
		}
		else if (ret > 0)
			process_message(session, msgbuff);
	}
}

void				worker_check_results(t_wmgr *session)
{
	pthread_mutex_lock(&session->tpool->results_mtx);
	if (session->results.result_cnt)
	{
		printf("sending results\n");
		if (send_results(session) == FAILURE)
			hermes_error(FAILURE, "results unsent");
	}
	else
	{
		printf("no results to send\n");
	}
	pthread_mutex_unlock(&session->tpool->results_mtx);
}

int					worker_loop(t_wmgr *session)
{
	struct pollfd	fds[1];

	memset(&fds, 0, sizeof(fds));
	fds[0].fd = session->sock;
	fds[0].events = POLLIN;
	printf("worker loop start\n");
	while (session->stat.running == true)
	{
		signal(SIGINT, sig_kill);
		poll_mgr_messages(session, (struct pollfd *) &fds);
		if (session->stat.initilized == true &&
			session->stat.has_work == false &&
				session->stat.work_requested == false)
		{
			printf("sending work request\n");
			hermes_sendmsgf(session->sock, msg_tc(T_WRK_REQ, C_WRK_REQ), NULL);
			session->stat.work_requested = true;
			printf("work request sent\n");
		}
		if (session->tpool)
		{
			pthread_mutex_lock(&session->tpool->amt_working_mtx);
			if (session->tpool->amt_working != session->tpool->tcount)
			{
				pthread_mutex_unlock(&session->tpool->amt_working_mtx);
				pthread_mutex_lock(&session->tpool->work_pool_mtx);
				if (session->targets.total == 0)
				{
					printf("setting \"has work\" to false\n");
					session->stat.has_work = false;
				}
				pthread_mutex_unlock(&session->tpool->work_pool_mtx);
			}
			else
				pthread_mutex_unlock(&session->tpool->amt_working_mtx);
			/* Check if we need to send results to manager */
			worker_check_results(session);
		}
	}
	return (SUCCESS);
}
