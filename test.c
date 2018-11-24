#include "incl/hermes.h"


int				main(void)
{
	t_iface		iface;

	iface.name = "wlp60s0";
	get_iface_info(&iface);
}

/* testing threads/hashtables */
//int			main(void)
//{
//	t_thread_pool	pool = {0};
//	t_env			env  = {0};
//	t_thread		thread  = {0};
//	t_hashtbl		tbl  = {0};
//	t_ip4rng		*rng  = {0};
//	t_targetset		targets  = {0};
//	t_result		*tmp;
//
//	env.ports.total = 1024;
//	rng = new_ip4range();
//	rng->start = 108736;
//	rng->end = ip4_increment(rng->start, 128);
//	rng->size = 128;
//	list_add_head(&targets.iprngs, (void **)&rng);
//	targets.total = 128;
//	targets.rng_cnt = 1;
//	pool.env = &env;
//	thread.pool = &pool;
//	thread.hstgrp = memalloc(sizeof(t_result *) * (THRD_HSTGRP_MAX + 1));
//	thread.lookup = &tbl;
//	tbl.buckets = memalloc(sizeof(t_hashbkt) * (THRD_HSTGRP_MAX));
//	tbl.bkt_cnt = THRD_HSTGRP_MAX;
//	tbl.prb_max = tbl.bkt_cnt;
//	targetset_to_hstgrp(&targets, &thread, &env);
//	add_results_to_lookup(&thread, targets.total);
//
//
//
//	for (size_t i = 0; i < targets.total; i++)
//	{
//		if (hashtbl_get(thread.lookup, (uint8_t *)&thread.hstgrp[i]->ip, 4, (void **)&tmp) == true)
//			printf("thread %d found result %ld %s\n", thread.id, i, inet_ntoa(tmp->ip));
//	}
//	for (size_t i = 0; i < targets.total; i++)
//	{
//		printf("thread %d deleting %s ...", thread.id, inet_ntoa(thread.hstgrp[i]->ip));
//		if (hashtbl_rm(thread.lookup, (uint8_t *)&thread.hstgrp[i]->ip, 4) == true)
//			printf("done\n");
//		else
//			printf("failed\n");
//	}
//
//	return (0);
//}