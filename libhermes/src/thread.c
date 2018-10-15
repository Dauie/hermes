#include "../incl/libhermes.h"

bool		thread_do_work(t_thread *thread)
{

}

int			thread_init(t_thread *thread, uint16_t id)
{
	thread->id = id;
	return (pthread_create(
				&thread->thread,
				NULL,
				(void*)thread_do_work,
				thread->thread
			));
}

t_thrpool	*thrpool_init(uint16_t num)
{
	uint16_t 	i;
	t_thrpool	*pool;
	uint8_t 	retries;

	if (!(pool = (t_thrpool*)memalloc(sizeof(t_thread))))
		return (NULL);
	if (!(pool->threads = (t_thread**)memalloc(sizeof(t_thread) * num)))
		return (NULL);
	pool->thr_count = num;
	i = 0;
	retries = 3;
	while (i < num && retries)
	{
		if (!thread_init(pool->threads[i], i))
		{
			i++;
			retries = 3;
		}
		else
			retries--;
	}
	return (pool);
}

void		thrpool_add_work()
{

}