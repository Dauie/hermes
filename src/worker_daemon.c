#include "../incl/hermes.h"

static int				accept_wrapper(t_wmgr *session, int lsock)
{
	uint				cslen;

	cslen = sizeof(sockaddr_in);
	if ((session->sock = accept(lsock, (sockaddr*)&session->sin, &cslen)) == -1)
		return (hermes_error(FAILURE, "accept() %s", strerror(errno)));
	return (SUCCESS);
}

static int				daemon_loop(t_wmgr *session, int lsock)
{
	while (session->stat.running)
	{
		if (accept_wrapper(session, lsock) == FAILURE)
			continue;
		if ((session->id = fork()) < 0)
			hermes_error(EXIT_FAILURE, "fork() %s", strerror(errno));
		else if (session->id > 0)
		{
			signal(SIGCHLD,SIG_IGN);
			close(session->sock);
		}
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
		hermes_error(FAILURE, "setsockopt() %s", strerror(errno));
}

void					destroy_worker_session(t_wmgr	**session)
{
	if ((*session)->results.results)
		del_list(&(*session)->results.results, true);
	if ((*session)->targets.ips)
		del_list(&(*session)->targets.ips, true);
	if ((*session)->targets.iprngs)
		del_list(&(*session)->targets.iprngs, true);
	if ((*session)->env.ports.ports)
		del_list(&(*session)->env.ports.ports, true);
	if ((*session)->env.ports.prtrngs)
		del_list(&(*session)->env.ports.prtrngs, true);

	if ((*session)->env.ack_ports)
	{
		if ((*session)->env.ack_ports->ports)
			del_list(&(*session)->env.ack_ports->ports, true);
		if ((*session)->env.ack_ports->prtrngs)
			del_list(&(*session)->env.ack_ports->prtrngs, true);
		free((*session)->env.ack_ports);
	}

	if ((*session)->env.syn_ports)
	{
		if ((*session)->env.syn_ports->ports)
			del_list(&(*session)->env.syn_ports->ports, true);
		if ((*session)->env.syn_ports->prtrngs)
			del_list(&(*session)->env.syn_ports->prtrngs, true);
		free((*session)->env.syn_ports);
	}

	if ((*session)->env.udp_ports)
	{
		if ((*session)->env.udp_ports->ports)
			del_list(&(*session)->env.udp_ports->ports, true);
		if ((*session)->env.udp_ports->prtrngs)
			del_list(&(*session)->env.udp_ports->prtrngs, true);
		free((*session)->env.udp_ports);
	}
	if ((*session)->tpool)
		free((*session)->tpool);
	free(*session);
}

int						hermes_daemon(int port)
{
	int					lsock;
	t_wmgr				*session;
	struct protoent		*proto;

	if (!(session = memalloc(sizeof(t_wmgr))))
		return (hermes_error(EXIT_FAILURE, "malloc() %s", strerror(errno)));
	session->stat.running = true;
	/* TODO add signal handlers */
	if ((proto = getprotobyname("tcp")) == NULL)
		hermes_error(EXIT_FAILURE, "getprotobyname() %s", strerror(errno));
	if ((lsock = socket(PF_INET, SOCK_STREAM, proto->p_proto)) == -1)
		hermes_error(EXIT_FAILURE, "socket() %s", strerror(errno));
	setsockopt_wrapper(lsock);
	session->sin.sin_family = AF_INET;
	session->sin.sin_port = htons(port);
	session->sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(lsock, (const sockaddr *)&session->sin, sizeof(session->sin)) < 0)
		hermes_error(EXIT_FAILURE, "bind() %s", strerror(errno));
	if (listen(lsock, 1) == -1)
		hermes_error(EXIT_FAILURE, "listen() %s", strerror(errno));
	printf("Daemon running on port: %d\n", port);
	daemon_loop(session, lsock);
	destroy_worker_session(&session);
	exit(EXIT_SUCCESS);
}
