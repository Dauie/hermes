# include "../incl/hermes.h"

# define LRGBUFF (8192)

typedef struct s_nlreq
{
	struct nlmsghdr hdr;
	struct rtgenmsg gen;
}				t_nlreq;

typedef struct s_nlpair
{
	int (*f)(t_iface *, struct nlmsghdr *);
	uint16_t		type;
}				t_nlpair;

/* OK */
int					parse_l2_resp(t_iface *info, struct nlmsghdr *hdr)
{
	bool				found;
	struct ifinfomsg	*iface;
	struct rtattr		*attrib;
	int					len;
	uint8_t				*data;

	iface = NLMSG_DATA(hdr);
	found = false;
	len = hdr->nlmsg_len - NLMSG_LENGTH(sizeof(struct ifinfomsg));
	attrib = IFLA_RTA(iface);
	while (RTA_OK(attrib, len))
	{
		if (attrib->rta_type == IFLA_IFNAME)
		{
			data = RTA_DATA(attrib);
			if (strcmp((char *)data, info->name) == 0)
			{
				info->inx = iface->ifi_index;
				found = true;
			}
			else
				break;
		}
		if (found == true && attrib->rta_type == IFLA_ADDRESS)
		{
			data = RTA_DATA(attrib);
			memcpy(info->if_hwaddr, data, ETH_ALEN);
			break;
		}
		attrib = RTA_NEXT(attrib, len);
	}
	return (found ? SUCCESS : FAILURE);
}

int					parse_l3resp(t_iface *info, struct nlmsghdr *hdr)
{
	bool				found;
	struct ifaddrmsg	*iface;
	struct rtattr		*attrib;
	int					len;
	uint8_t				*data;

	iface = NLMSG_DATA(hdr);
	if (iface->ifa_index != info->inx)
		return (FAILURE);
	found = false;
	len = hdr->nlmsg_len - NLMSG_LENGTH(sizeof(struct ifaddrmsg));

	attrib = IFA_RTA(iface);
	while (RTA_OK(attrib, len))
	{
		if (attrib->rta_type == IFA_LOCAL)
		{
			data = RTA_DATA(attrib);
			info->if_ip.s_addr = *(uint32_t *)data;
			found = true;
			break;
		}
		attrib = RTA_NEXT(attrib, len);
	}
	return (found ? SUCCESS : FAILURE);
}



int						parse_routeresp(t_iface *info, struct nlmsghdr *hdr)
{
	bool				found;
	struct rtmsg		*route;
	struct rtattr		*attrib;
	int					len;

	found = false;
	route = NLMSG_DATA(hdr);
	if (route->rtm_family != AF_INET || route->rtm_table != RT_TABLE_MAIN)
		return (FAILURE);
	len = hdr->nlmsg_len - NLMSG_LENGTH(sizeof(struct rtmsg));
	attrib = RTM_RTA(route);
	while (RTA_OK(attrib, len))
	{
		if (attrib->rta_type == RTA_SRC)
			break;
		if (attrib->rta_type == RTA_OIF)
		{
			if (*(uint32_t *)RTA_DATA(attrib) != info->inx)
				break;
		}
		if (attrib->rta_type == RTA_GATEWAY)
		{
			info->gw_ip.s_addr = *(uint32_t *)RTA_DATA(attrib);
			printf("%s\n", inet_ntoa(info->gw_ip));
			found = true;
		}
		attrib = RTA_NEXT(attrib, len);
	}
	return (found ? SUCCESS : FAILURE);
}

int						parse_neighresp(t_iface *info, struct nlmsghdr *hdr)
{
	uint32_t			addr;
	bool				found;
	struct ndmsg		*neigh;
	struct rtattr		*attrib;
	void				*data;
	int					len;

	found = false;
	neigh = NLMSG_DATA(hdr);
	if (neigh->ndm_ifindex != (int)info->inx)
		return (FAILURE);
	len = hdr->nlmsg_len - NLMSG_LENGTH(sizeof(struct ndmsg));
	attrib = NDA_RTA(neigh);
	while (RTA_OK(attrib, len))
	{
		if (attrib->rta_type == NDA_DST)
		{
			addr = *(uint32_t *)RTA_DATA(attrib);
			if (addr != info->gw_ip.s_addr)
				break;
		}
		if (attrib->rta_type == NDA_LLADDR)
		{
			data = RTA_DATA(attrib);
			memcpy(info->gw_hwaddr, data, ETH_ALEN);
			found = true;
		}
		printf("code: %d\n", attrib->rta_type);
		attrib = RTA_NEXT(attrib, len);
	}
	return (found ? SUCCESS : FAILURE);
}

int						send_nlinforeq(int sock, struct sockaddr_nl *loc,
										  struct sockaddr_nl *kernel, uint16_t type)
{
	static uint32_t		nlmsg_seq;
	struct msghdr		rtnlmsg;
	struct iovec		io;
	t_nlreq				req;

	bzero(&rtnlmsg, sizeof(struct msghdr));
	bzero(&req, sizeof(t_nlreq));
	bzero(&io, sizeof(struct iovec));

	/* fill in our request */
	req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
	req.hdr.nlmsg_pid = loc->nl_pid;
	req.hdr.nlmsg_type = type;
	req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
	req.hdr.nlmsg_seq = nlmsg_seq++;
	req.gen.rtgen_family = AF_INET;

	/* put request in our iovec */
	io.iov_base = &req;
	io.iov_len = req.hdr.nlmsg_len;

	rtnlmsg.msg_iov = &io;
	rtnlmsg.msg_iovlen = 1;
	rtnlmsg.msg_name = kernel;
	rtnlmsg.msg_namelen = sizeof(struct sockaddr_nl);

	/* send our request to kernel */
	if (sendmsg(sock, &rtnlmsg, 0) < 0)
		return (hermes_error(FAILURE, "sendmsg() %s", strerror(errno)));
	return (SUCCESS);
}

int						recv_parse_req(int sock, t_iface *info, struct sockaddr_nl *kernel, t_nlpair *conf)
{
	/*receive our response*/
	uint8_t			respbuff[LRGBUFF] = {0};
	bool			run = true;
	bool			found = false;
	ssize_t			len = 0;
	struct nlmsghdr	*msg;
	struct iovec	io_rply;
	struct msghdr	rtnl_rply;

	while (run)
	{
		len = 0;
		msg = NULL;
		bzero(respbuff, LRGBUFF);
		bzero(&io_rply, sizeof(io_rply));
		bzero(&rtnl_rply, sizeof(rtnl_rply));
		/* prepare iovec for response */
		io_rply.iov_base = respbuff;
		io_rply.iov_len = LRGBUFF;
		rtnl_rply.msg_iov = &io_rply;
		rtnl_rply.msg_iovlen = 1;
		rtnl_rply.msg_name = kernel;
		rtnl_rply.msg_namelen = sizeof(struct sockaddr_nl);
		if ((len = recvmsg(sock, &rtnl_rply, 0)) < 0)
			return (hermes_error(FAILURE, "recvmsg() %s", strerror(errno)));
		if (len)
		{
			msg = (struct nlmsghdr *) respbuff;
			while (NLMSG_OK(msg, len))
			{
				if (msg->nlmsg_type == NLMSG_DONE)
				{
					run = false;
					break;
				}
				else if (found != true && msg->nlmsg_type == conf->type)
				{
					if (conf->f(info, msg) == SUCCESS)
						found = true;
				}
				msg = NLMSG_NEXT(msg, len);
			}
		}
	}
	return (found ? SUCCESS : FAILURE);
}

int						get_iface_info(t_iface *info)
{
	struct sockaddr_nl	kernel;
	int					sock;
	struct sockaddr_nl	loc;
	t_nlpair			conf;


	/* setup */
	/* Make our socket for kernel comms */
	if ((sock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
		return (hermes_error(FAILURE, "socket() AF_NETLINK %s", strerror(errno)));

	/* Bind our socket to our process */
	memset(&loc, 0, sizeof(struct sockaddr_nl));
	loc.nl_family = AF_NETLINK;
	loc.nl_pid = (uint32_t)getpid();
	loc.nl_groups = 0; /* multicast group id for mc operations */
	if (bind(sock, (struct sockaddr*)&loc, sizeof(loc)) < 0)
		return (hermes_error(FAILURE, "bind() %s", strerror(errno)));

	/* set msg dst to kernel */
	bzero(&kernel, sizeof(struct sockaddr_nl));
	kernel.nl_family = AF_NETLINK;

	/* Get mac address & index of our interface */
	if (send_nlinforeq(sock, &loc, &kernel, RTM_GETLINK) == FAILURE)
		return (FAILURE);
	conf.f = parse_l2_resp;
	conf.type = RTM_NEWLINK;
	if (recv_parse_req(sock, info, &kernel, &conf) == FAILURE)
		return (hermes_error(FAILURE, "failed to find info for interface %s", info->name));

	/* Get interface ip address */
	if (send_nlinforeq(sock, &loc, &kernel, RTM_GETADDR) == FAILURE)
		return (FAILURE);
	conf.f = parse_l3resp;
	conf.type = RTM_NEWADDR;
	if (recv_parse_req(sock, info, &kernel, &conf) == FAILURE)
		return (FAILURE);

	/* get our default gateway's ip */
	if (send_nlinforeq(sock, &loc, &kernel, RTM_GETROUTE) == FAILURE)
		return (FAILURE);
	conf.f = parse_routeresp;
	conf.type = RTM_NEWROUTE;
	if (recv_parse_req(sock, info, &kernel, &conf) == FAILURE)
		return (FAILURE);

	/* get default gateway's mac address*/
	if (send_nlinforeq(sock, &loc, &kernel, RTM_GETNEIGH) == FAILURE)
		return (FAILURE);
	conf.f = parse_neighresp;
	conf.type = RTM_NEWNEIGH;
	if (recv_parse_req(sock, info, &kernel, &conf) == FAILURE)
		return (FAILURE);

	if (info->gw_ip.s_addr == info->if_ip.s_addr)
		printf("They really are equal\n");
	printf("index %d | mac: ", info->inx);
	for (int i = 0; i < ETH_ALEN; i++)
		printf("%0x:", info->if_hwaddr[i]);
	printf("| ip: %s ", inet_ntoa(info->if_ip));
	printf("| gwip: %s | gwmac: ", inet_ntoa(info->gw_ip));
	for (int i = 0; i < ETH_ALEN; i++)
		printf("%0x:", info->gw_hwaddr[i]);
	printf("\n");
	return (SUCCESS);
}
