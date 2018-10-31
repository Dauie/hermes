# include "../incl/hermes.h"

bool                new_tsem(t_sem **sem)
{
	if (!(*sem = (t_sem*)memalloc(sizeof(t_sem))))
		return (false);
	return (true);
}

bool                new_tpool(t_thread_pool **pool)
{
	if (!(*pool = (t_thread_pool*)memalloc(sizeof(t_thread_pool))))
		return (false);
	if (new_tsem(&(*pool)->tsem))
		return (false);
	return (true);
}

void                tpool_event(t_thread_pool *pool)
{
	int i;

	i = -1;
	pthread_mutex_lock(&pool->tsem->stop);
	while (++i < pool->tcount)
	{
		pool->threads[i].working = true;
		pthread_mutex_lock(&pool->amt_working_mtx);
		pool->amt_working++;
		pthread_mutex_unlock(&pool->amt_working_mtx);
	}
	pthread_cond_broadcast(&pool->tsem->wait);
	pthread_mutex_unlock(&pool->tsem->stop);
}

void                tpool_wait(t_thread_pool *pool)
{
	pthread_mutex_lock(&pool->tsem->stop);
	while (pool->amt_working == 0)
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
	sleep(1);
	if (pool->threads)
		free(pool->threads);
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
			printf("thread %d taking work\n", thread->id);
			transfer_work(&work, thread->pool->work_pool, thread->amt);
			printf("thread %d finished taking work\n", thread->id);
			pthread_mutex_unlock(&thread->pool->work_pool_mtx);
			thread->amt *= (thread->amt < 512) ? 2 : 1;
			thread->working = true;
			pthread_mutex_lock(&thread->pool->amt_working_mtx);
			thread->pool->amt_working += 1;
			pthread_mutex_unlock(&thread->pool->amt_working_mtx);
			printf("thread %d entering scan\n", thread->id);
			run_scan(thread->pool->env, &work,
					 thread->pool->results, &thread->pool->results_mtx);
			printf("thread %d left scan\n", thread->id);
			printf("ip %u done\n",
			       ((t_result*)thread->pool->results->results->data)->ip.s_addr);
			thread->working = false;
			pthread_mutex_lock(&thread->pool->amt_working_mtx);
			thread->pool->amt_working -= 1;
			pthread_mutex_unlock(&thread->pool->amt_working_mtx);
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
	t_thread_pool			*pool;

	i = -1;
	if (!(pool = memalloc(sizeof(t_thread_pool))))
		return (NULL);
	if (!(pool->threads = memalloc(sizeof(t_thread) * env->opts.thread_count)))
		return (NULL);
	pthread_mutex_init(&pool->work_pool_mtx, NULL);
	pthread_mutex_init(&pool->results_mtx, NULL);
	pthread_mutex_init(&pool->amt_working_mtx, NULL);
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
		pool->threads[i].working = false;
		pthread_create(&pool->threads[i].thread, NULL, thread_loop, &pool->threads[i]);
	}
	return (pool);
}

