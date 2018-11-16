#include <net/if.h>
# include "../incl/hermes.h"

bool                new_tsem(t_sem **sem)
{

	if (!(sem = (t_sem**)memalloc(sizeof(t_sem))))
		return (false);
	return (true);
}

void                tpool_event(t_thread_pool *pool)
{
	uint16_t i;

	i = 0;
	pthread_mutex_lock(&pool->tsem->stop);
	while (i < pool->tcount)
	{
		pool->threads[i].working = true;
		i++;
	}
	pthread_cond_broadcast(&pool->tsem->wait);
	pthread_mutex_unlock(&pool->tsem->stop);
	pool->amt_working = i;
}

void                tpool_wait(t_thread_pool *pool)
{
	pthread_mutex_lock(&pool->tsem->stop);
	while (pool->work_pool->total == 0)
		pthread_cond_wait(&pool->tsem->wait, &pool->tsem->stop);
	pthread_mutex_unlock(&pool->tsem->stop);
}

void				tpool_kill(t_thread_pool *pool)
{
	int				i;

	i = -1;
	if (!pool)
		return ;
	while (++i < pool->tcount)
		pool->threads[i].alive = false;
	if (pool->threads)
		free(pool->threads);
}
void					kill_threadpool(t_thread_pool **pool)
{
	int					i;

	i = -1;
	while (++i < (*pool)->thread_amt)
		(*pool)->threads[i].alive = false;
	sleep(1);
	if ((*pool)->threads)
		free((*pool)->threads);
	free(*pool);
}

int						find_live_interface_indx(int sock)
{
	struct ifreq		ifr;
	struct ifaddrs		*addrs;

	bzero(&ifr, sizeof(struct ifreq));
	if (getifaddrs(&addrs) == -1)
	{
		hermes_error(FAILURE, "getifaddrs() %s", strerror(errno));
		return (-1);
	}
	while (addrs)
	{
		if (addrs->ifa_addr &&
			(addrs->ifa_flags & IFF_UP) &&
				(addrs->ifa_flags & IFF_RUNNING))
		{
			if (addrs->ifa_addr->sa_family == AF_INET &&
				((struct sockaddr_in *)addrs->ifa_addr)->sin_addr.s_addr !=
						htonl(LOOPBACK_ADDR))
			{
				strcpy(ifr.ifr_ifrn.ifrn_name, addrs->ifa_name);
				if ((ioctl(sock, SIOCGIFINDEX, &ifr)) < 0)
					return (hermes_error(FAILURE, "ioctl() %s", strerror(errno)));
				break;
			}
		}
	}
	freeifaddrs(addrs);
	return (ifr.ifr_ifindex);
}

/*
** Transmission process is similar to capture as shown below.
**
**[setup]          socket() -------> creation of the transmission socket
**                 setsockopt() ---> allocation of the circular buffer (ring)
**                                   option: PACKET_TX_RING
**                 bind() ---------> bind transmission socket with a network interface
**                 mmap() ---------> mapping of the allocated buffer to the
**                                   user process
**
**[transmission]   poll() ---------> wait for free packets (optional)
**                 send() ---------> send all packets that are set as ready in
**                                   the ring
**                                   The flag MSG_DONTWAIT can be used to return
**                                   before end of transfer.
**
**[shutdown]  close() --------> destruction of the transmission socket and
**                              deallocation of all associated resources.
*/

//int						prepare_pcap_rx()
//{
//
//}

int						prepare_thread_tx_ring(t_thread *thread)
{
	struct tpacket_req tpr;
	struct sockaddr_ll sll_loc;
	int ifinx;
	size_t ring_size;

	ring_size = 0;
	/* TODO figure out block/frame sizes for mmap tx_ring */
	memset(&sll_loc, 0, sizeof(struct sockaddr_ll));
	thread->sock = socket(PF_PACKET, SOCK_DGRAM,
						  htons(ETH_P_ALL));                /* step 1 */
	if (thread->sock == -1) {
		thread->alive = false;
		return (hermes_error(FAILURE, "socket() %s", strerror(errno)));
	}
	setsockopt(thread->sock, SOL_SOCKET, IP_HDRINCL,);
	memset(&tpr, 0, sizeof(struct tpacket_req));
	(void) tpr;
	memset(&sll_loc, 0, sizeof(struct sockaddr_ll));
	sll_loc.sll_family = AF_PACKET;
	sll_loc.sll_protocol = htons(ETH_P_ALL);
	if ((ifinx = find_live_interface_indx(thread->sock)) < 0) {
		thread->alive = false;
		return (FAILURE);
	}
	sll_loc.sll_ifindex = ifinx;
	/* TODO possibly increase iface mtu */
	if (bind(thread->sock, (sockaddr *) &sll_loc, sizeof(struct sockaddr_ll)) ==
		-1) {
		thread->alive = false;
		return (hermes_error(FAILURE, "bind() %s", strerror(errno)));
	}
	mmap(0, ring_size, PROT_READ | PROT_WRITE, MAP_SHARED, thread->sock, 0);
	return (SUCCESS);

	if (pool->threads)
		free(pool->threads);
}

void					*thread_loop(void *thrd)
{
	t_targetset		work;
	t_thread_pool   *tpool;
	t_thread		*thread;

	thread = (t_thread*)thrd;
	tpool = thread->pool;
	memset(&work, 0, sizeof(t_targetset));
	while (thread->alive)
	{
		tpool_wait(tpool);
		pthread_mutex_lock(&thread->pool->work_pool_mtx);
		if (thread->pool->work_pool->total > 0)
		{
			transfer_work(&work, thread->pool->work_pool, thread->amt);
			pthread_mutex_unlock(&thread->pool->work_pool_mtx);
			thread->amt *= (thread->amt < 64) ? 2 : 1;
			if (work.total > 0)
			{
				pthread_mutex_lock(&thread->pool->amt_working_mtx);
				thread->pool->amt_working += 1;
				pthread_mutex_unlock(&thread->pool->amt_working_mtx);
				test_run_scan(thread->pool->env, &work, thread->pool->results,
						&thread->pool->results_mtx);
				pthread_mutex_lock(&thread->pool->amt_working_mtx);
				thread->pool->amt_working -= 1;
				pthread_mutex_unlock(&thread->pool->amt_working_mtx);
			}
		}
		else
		{
			pthread_mutex_unlock(&thread->pool->work_pool_mtx);
		}
	}
	return (NULL);
}

t_thread_pool			*init_threadpool(t_env *env, t_targetset *workpool,
											t_resultset *results)
{
	int					i;
	t_thread_pool		*pool;

	i = -1;
	if (!(pool = memalloc(sizeof(t_thread_pool))))
		return (NULL);
	if (!(pool->threads = memalloc(sizeof(t_thread) * env->opts.thread_count)))
		return (NULL);
	if (pthread_mutex_init(&pool->work_pool_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	if (pthread_mutex_init(&pool->results_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	if (pthread_mutex_init(&pool->amt_working_mtx, NULL) != 0)
		hermes_error(EXIT_FAILURE, "pthread_mutex_init()");
	pool->thread_amt = env->opts.thread_count;
	pool->reqest_amt = pool->thread_amt;
	if (!(pool->tsem = memalloc(sizeof(t_sem))))
		return (NULL);
	pthread_mutex_init(&pool->work_pool_mtx, NULL);
	pthread_mutex_init(&pool->results_mtx, NULL);
	pthread_mutex_init(&pool->amt_working_mtx, NULL);
	pthread_mutex_init(&pool->tsem->stop, NULL);
	pthread_cond_init(&pool->tsem->wait, NULL);
	pool->tcount = env->opts.thread_count;
	pool->reqest_amt = pool->tcount;
	pool->results = results;
	pool->work_pool = workpool;
	pool->env = env;
	while (++i < pool->thread_amt)
	{
		pool->threads[i].id = (uint8_t)(i + 2);
		pool->threads[i].amt = 1;
		pool->threads[i].pool = pool;
		pool->threads[i].alive = true;
		pthread_create(&pool->threads[i].thread, NULL, thread_loop, &pool->threads[i]);
	}
	return (pool);
}
