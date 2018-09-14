

#include "../incl/hermese.h"

typedef struct			s_daemon_session /* worker daemon session */
{
	int					run;
	int					pid;
	int					lsock;	/* listen socket */
	int					csock;	/* client socket */
	struct sockaddr_in	sin;
}						t_dsession;

static int				accept_wrapper(t_dsession *session)
{
	uint		cslen;

	cslen = sizeof(struct sockaddr_in);
	if ((session->csock = accept(session->lsock,
							(struct sockaddr*)&session->sin, &cslen)) == -1)
		return (FAILURE);
	/* ^ TODO add non-fatal hermese_error() call for max amount of connections */
	return (SUCCESS);
}

static int					daemon_loop(t_dsession *session)
{
	while (session->run)
	{
		if (accept_wrapper(session) == FAILURE)
			continue;
		if ((session->pid = fork()) < 0)
		{
			close(session->csock);
			dprintf(STDERR_FILENO, "hermese: Error fork(). %s\n", strerror(errno));
		}
		else if (session->pid > 0)
			close(session->csock);
		else
		{
			close(session->lsock);
			worker_loop(session->csock);
		}
	}
}

static void				setsockopt_wrapper(t_dsession *session)
{
	int				opt;

	opt = TRUE;
	if (setsockopt(session->lsock, SOL_SOCKET, SO_REUSEADDR,
				   (char *)&opt, sizeof(opt)) < 0)
	{
		/* TODO hermese_error() */
		dprintf(STDERR_FILENO, "hermese: Error setsockopt(). %s", strerror(errno));
		exit(FAILURE);
	}
}


static int				worker_daemon(int port)
{
	t_dsession			*session;
	struct protoent		*proto;

	if (!(session = memalloc(sizeof(t_dsession))))
	{
		dprintf(STDERR_FILENO, "hermese: Error malloc(). %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		/* TODO hermese_error() */
	}
	session->run = TRUE;
	/* TODO add signal handlers */
	if ((proto = getprotobyname("tcp")) == NULL)
	{
		dprintf(STDERR_FILENO, "hermese: Error getprotobyname(). %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		/* TODO hermese_error() */
	}
	if ((session->lsock = socket(PF_INET, SOCK_STREAM, proto->p_proto)) == -1)
	{
		dprintf(STDERR_FILENO, "hermese: Error socket(). %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		/*TODO hermese_error() */
	}
	setsockopt_wrapper(session);
	session->sin.sin_family = AF_INET;
	session->sin.sin_port = htons(port);
	session->sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(session->lsock, (const struct sockaddr *)&session->sin,
			sizeof(session->sin)) < 0)
	{
		dprintf(STDERR_FILENO, "hermese: Error bind(). %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		/*TODO hermese_error() */
	}
	if (listen(session->lsock, 1) == -1)
	{
		dprintf(STDERR_FILENO, "hermese: Error listen(). %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		/*TODO hermese_error() */
	}
	return (daemon_loop(session));
}