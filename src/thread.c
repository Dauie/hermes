# include "../incl/hermes.h"

void				kill_threadpool(t_thread_pool **pool)
{
	int				i;

	i = -1;
	if (!pool)
		return ;
	while (++i < (*pool)->tcount)
		(*pool)->threads[i].alive = false;
	sleep(1);
	if ((*pool)->threads)
		free((*pool)->threads);
	free(*pool);
}

void				*thread_loop(void *thrd)
{
	t_targetset		work;
	t_thread		*thread;

	thread = (t_thread *)thrd;
	memset(&work, 0, sizeof(t_targetset));
	while (thread->alive)
	{
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
				test_run_scan(thread->pool->env, &work,
							  thread->pool->results, &thread->pool->results_mtx);
				pthread_mutex_lock(&thread->pool->amt_working_mtx);
				thread->pool->amt_working -= 1;
				pthread_mutex_unlock(&thread->pool->amt_working_mtx);
			}
		}
		else
		{
			pthread_mutex_unlock(&thread->pool->work_pool_mtx);
			sleep(1);
		}
	}
	return (NULL);
}

t_thread_pool			*init_threadpool(t_env *env, t_targetset *workpool, t_resultset *results)
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
	pool->tcount = env->opts.thread_count;
	pool->reqest_amt = pool->tcount;
	pool->results = results;
	pool->work_pool = workpool;
	pool->env = env;
	while (++i < pool->tcount)
	{
		pool->threads[i].id = (uint8_t)(i + 2);
		pool->threads[i].amt = 1;
		pool->threads[i].pool = pool;
		pool->threads[i].alive = true;
		pthread_create(&pool->threads[i].thread, NULL, thread_loop, &pool->threads[i]);
	}
	return (pool);
}
