#include "../incl/hermes.h"
#include "../incl/defined.h"



static int				accept_wrapper(t_wsession *session, int lsock)
{
	uint				cslen;

	cslen = sizeof(sockaddr_in);
	if ((session->sock = accept(lsock, (sockaddr*)&session->sin, &cslen)) == -1)
		return (FAILURE);
	/* ^ TODO add non-fatal hermes_error() call for max amount of connections */
	return (SUCCESS);
}

static int				daemon_loop(t_wsession *session, int lsock)
{
	while (session->stat.run)
	{
		if (accept_wrapper(session, lsock) == FAILURE)
			continue;
		if ((session->pid = fork()) < 0)
			hermes_error(EXIT_FAILURE, 2, "fork()", strerror(errno));
		else if (session->pid > 0)
			close(session->sock);
		/*TODO: add wait() to protect from zombie children*/
		else
		{
			close(lsock);
			worker_loop(session);
		}
	}
	return (SUCCESS);
}

static void				setsockopt_wrapper(int lsock)
{
	int					opt;

	opt = true;
	if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR,
				   (char *)&opt, sizeof(opt)) < 0)
		hermes_error(EXIT_FAILURE, 2, "setsockopt()", strerror(errno));
}

int						worker_daemon(int port)
{
	int					lsock;
	t_wsession			*session;
	struct protoent		*proto;

	if (!(session = memalloc(sizeof(t_wsession))))
		return (hermes_error(EXIT_FAILURE, 2, "malloc()", strerror(errno)));
	session->stat.run = true;
	/* TODO add signal handlers */
	if ((proto = getprotobyname("tcp")) == NULL)
		hermes_error(EXIT_FAILURE, 2, "getprotobyname()", strerror(errno));
	if ((lsock = socket(PF_INET, SOCK_STREAM, proto->p_proto)) == -1)
		hermes_error(EXIT_FAILURE, 2, "socket()", strerror(errno));
	setsockopt_wrapper(lsock);
	session->sin.sin_family = AF_INET;
	session->sin.sin_port = htons(port);
	session->sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(lsock, (const sockaddr *)&session->sin,
			sizeof(session->sin)) < 0)
		hermes_error(EXIT_FAILURE, 2, "bind()", strerror(errno));
	if (listen(lsock, 1) == -1)
		hermes_error(EXIT_FAILURE, 2, "listen()", strerror(errno));
	printf("starting daemon\n");
	return (daemon_loop(session, lsock));
}
