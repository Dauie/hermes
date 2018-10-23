#include "../incl/libhermes.h"



int			thread_init(t_thread *thread, uint16_t id, void (go)(void*))
{
	thread->id = id;
	return (pthread_create(
				&thread->thread,
				NULL,
				(void*)go,
				thread->thread
			));
}

t_thrpool	*thrpool_init(uint16_t num, void (go)(t_thread*, ))
{
	uint16_t 	i;
	t_thrpool	*pool;

	if (!(pool = (t_thrpool*)memalloc(sizeof(t_thread))))
		return (NULL);
	if (!(pool->threads = (t_thread**)memalloc(sizeof(t_thread) * num)))
		return (NULL);
	pool->thr_count = num;
	i = 0;
	while (i < num)
		if (!thread_init(pool->threads[i], i, go))
			i++;
	return (pool);
}

void		thrpool_add_work()
{

}