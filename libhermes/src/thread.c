#include "../incl/libhermes.h"

void		thread_wait(t_sem *wait)
{
	pthread_mutex_lock(&wait->mutex);
	while (!wait->work)
		pthread_cond_wait(&wait->cond, &wait->mutex);
	pthread_mutex_unlock(&wait->mutex);
}

t_thrpool		*thread_init(uint16_t id, void (*go)(void*))
{
	thread->id = id;

	if (!(thread->waiting = (t_sem*)memalloc(sizeof(t_sem))))
		return (hermes_error(FAILURE, "malloc() %s", strerror(errno)));
	thread->working = 0;
	thread->amt = 2;
	if (pthread_create(thread->thread, NULL, (void*)go, thread) != 0)
		return (FAILURE);
	if (pthread_detach(*thread->thread) != 0)
		return (FAILURE);
	return (SUCCESS);
}

t_thrpool	*thrpool_init(uint16_t num, void (*go)(t_thread*, void**))
{
	uint16_t 	i;
	t_thrpool	*pool;

	if (!(pool = (t_thrpool*)memalloc(sizeof(t_thread))))
		return (NULL);
	if (!(pool->threads = (t_thread*)memalloc(sizeof(t_thread) * num)))
		return (NULL);
	pool->thr_count = num;
	i = 0;
	while (i < num)
	{
		if (thread_init(&pool->threads[i], i, (void*)go) == SUCCESS)
			pool->threads[i].pool = pool;
		else
			// TODO : return hermes err and errno on failure
			return (NULL);
		i++;
	}
	return (pool);
}