#include "../incl/hermes.h"
#include "../incl/defined.h"

typedef struct			s_daemon_session /* worker daemon session */
{
	int					run;
	int					pid;
	int					lsock;	/* listen socket */
	int					csock;	/* client socket */
	struct sockaddr_in	sin;
}						t_dsession;

void                    worker_loop(int csock)
{
    char buffer[BUFF_SIZE];
    printf("Connected\n");
//    while (TRUE)
//    {
//        if (recvfrom(csock, buffer, BUFF_SIZE, 0) < 0)
//            hermes_error(INPUT_ERROR, TRUE, 2, "worker loop recv()", strerror(errno));
//
//    }
}

static int				accept_wrapper(t_dsession *session)
{
	uint		cslen;

	cslen = sizeof(struct sockaddr_in);
	if ((session->csock = accept(session->lsock,
							(struct sockaddr*)&session->sin, &cslen)) == -1)
		return (FAILURE);
	/* ^ TODO add non-fatal hermes_error() call for max amount of connections */
	return (SUCCESS);
}

static int					daemon_loop(t_dsession *session)
{
	while (session->run)
	{
		if (accept_wrapper(session) == FAILURE)
			continue;
		if ((session->pid = fork()) < 0)
			hermes_error(INPUT_ERROR, TRUE, 2, "fork()", strerror(errno));
		else if (session->pid > 0)
			close(session->csock);
		/*TODO: add wait() to protect from zombie children*/
		else
		{
			close(session->lsock);
//			worker_loop(session->csock);
		}
	}
	return (SUCCESS);
}

static void				setsockopt_wrapper(t_dsession *session)
{
	int				opt;

	opt = TRUE;
	if (setsockopt(session->lsock, SOL_SOCKET, SO_REUSEADDR,
				   (char *)&opt, sizeof(opt)) < 0)
		hermes_error(INPUT_ERROR, TRUE, 2, "setsockopt()", strerror(errno));
}


int						worker_daemon(int port)
{
	t_dsession			*session;
	struct protoent		*proto;

	if (!(session = memalloc(sizeof(t_dsession))))
		return (hermes_error(INPUT_ERROR, TRUE, 2, "malloc()", strerror(errno)));
	session->run = TRUE;
	/* TODO add signal handlers */
	if ((proto = getprotobyname("tcp")) == NULL)
		hermes_error(INPUT_ERROR, TRUE, 2, "getprotobyname()", strerror(errno));
	if ((session->lsock = socket(PF_INET, SOCK_STREAM, proto->p_proto)) == -1)
		hermes_error(INPUT_ERROR, TRUE, 2, "socket()", strerror(errno));
	setsockopt_wrapper(session);
	session->sin.sin_family = AF_INET;
	session->sin.sin_port = htons(port);
	session->sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(session->lsock, (const struct sockaddr *)&session->sin,
			sizeof(session->sin)) < 0)
		hermes_error(INPUT_ERROR, TRUE, 2, "bind()", strerror(errno));
	if (listen(session->lsock, 1) == -1)
		hermes_error(INPUT_ERROR, TRUE, 2, "listen()", strerror(errno));
	return (daemon_loop(session));
}

#ifdef TESTING
int main(void)
{
    int w_port;

    w_port = 4000;
    if (worker_daemon(w_port) < 0)
        hermes_error(INPUT_ERROR, TRUE, 2, "worker_daemon()", strerror(errno));
}
#endif