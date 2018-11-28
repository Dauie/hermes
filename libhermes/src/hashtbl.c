#include "../incl/libhermes.h"

int			fill_distance_to_initinx(t_hashtbl *tbl, uint16_t stored_i, uint16_t *distance)
{
	if(tbl->buckets[stored_i].data == NULL)
		return FAILURE;
	uint16_t init_i = tbl->buckets[stored_i].hash % tbl->bkt_cnt;
	if (init_i <= stored_i)
	{
		*distance = stored_i - init_i;
	}
	else
	{
		*distance = stored_i + (tbl->bkt_cnt - init_i);
	}
	return SUCCESS;
}

uint16_t 		jenkins_hash(const uint8_t *key, size_t len)
{
	size_t		i;
	uint16_t	hash;

	i = 0;
	hash = 0;
	while (i != len)
	{
		hash += key[i++];
		hash += hash << 10;
		hash += hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 1;
	hash += hash << 15;
	return (hash);
}

void			*new_hashtbl(size_t size)
{
	t_hashtbl	*ht;

	if (!(ht = memalloc(sizeof(t_hashtbl))))
		return (NULL);
	if (!(ht->buckets = memalloc(sizeof(t_hashbkt) * (size + 1))))
		return (NULL);
	ht->bkt_cnt = (uint16_t)size;
	ht->prb_max = (uint16_t)size;
	return (ht);
}

bool			hashtbl_rm(t_hashtbl *tbl, uint32_t key)
{
	uint16_t hash;
	uint16_t init_i;
	bool found;
	uint16_t cur_i;
	uint16_t prb_dist;
	uint16_t prev_i;
	uint16_t swap_i;
	uint16_t i;

	hash = jenkins_hash((uint8_t *)&key, sizeof(uint32_t));
	init_i = hash % tbl->bkt_cnt;
	found = false;
	cur_i = 0;
	prb_dist = 0;
	for (i = 0; i < tbl->bkt_cnt; i++)
	{
		cur_i = (uint16_t)(init_i + i) % tbl->bkt_cnt;
		fill_distance_to_initinx(tbl, cur_i, &prb_dist);
		if (tbl->buckets[cur_i].data == NULL || i > prb_dist) {
			break;
		}
		if (memcmp(&tbl->buckets[cur_i].key, &key, sizeof(uint32_t)) == 0) {
			found = true;
			break;
		}
	}
	if (found)
	{
		tbl->buckets[cur_i].data = NULL;
		tbl->buckets[cur_i].hash = 0;
		for (i = 1; i < tbl->bkt_cnt; i++)
		{
			prev_i = (uint16_t)(cur_i + i - 1) % tbl->bkt_cnt;
			swap_i = (cur_i + i) % tbl->bkt_cnt;
			if (tbl->buckets[swap_i].data == NULL)
			{
				tbl->buckets[prev_i].data = NULL;
				break;
			}
			if (fill_distance_to_initinx(tbl, swap_i, &prb_dist) != 0)
				hermes_error(FAILURE, "fill_distance_to_initinx()");
			if (prb_dist == 0)
			{
				tbl->buckets[prev_i].data = NULL;
				break;
			}
			tbl->buckets[prev_i].data = tbl->buckets[swap_i].data;
			tbl->buckets[prev_i].hash = tbl->buckets[swap_i].hash;
		}
		tbl->bkt_usd -= 1;
		return (true);
	}
	return (false);
}

bool 			hashtbl_add(t_hashtbl *tbl, uint32_t key,
							void **value)
{
	uint16_t	i;
	uint16_t	init_i;
	uint16_t	cur_i;

	uint16_t	prb_dist;
	uint16_t	prb_cur;

	uint16_t	hash_tmp;
	uint16_t	hash;

	uint32_t	key_tmp;

	void		*data;
	void		*data_tmp;
	int			num_swaps;


	if (tbl->bkt_usd == tbl->bkt_cnt)
		return (false);
	tbl->bkt_usd += 1;
	hash = jenkins_hash((uint8_t *)&key, sizeof(uint32_t));
	init_i = hash % tbl->bkt_cnt;
	prb_dist = 0;
	prb_cur = 0;
	data = *value;
	num_swaps = 0;
	for (i = 0; i < tbl->prb_max; i++)
	{
		cur_i = (init_i + i) % tbl->bkt_cnt;
		if (tbl->buckets[cur_i].data == NULL)
		{
			tbl->buckets[cur_i].key = key;
			tbl->buckets[cur_i].data = data;
			tbl->buckets[cur_i].hash = hash;
			break;
		}
		else
		{
			fill_distance_to_initinx(tbl, cur_i, &prb_dist);
			if (prb_cur > prb_dist)
			{
				data_tmp = tbl->buckets[cur_i].data;
				hash_tmp = tbl->buckets[cur_i].hash;
				key_tmp = tbl->buckets[cur_i].key;
				tbl->buckets[cur_i].data = data;
				tbl->buckets[cur_i].hash = hash;
				tbl->buckets[cur_i].key = key;
				data = data_tmp;
				hash = hash_tmp;
				key = key_tmp;
				prb_cur = prb_dist;
				num_swaps += 1;
			}
		}
		prb_cur++;
	}
	return (true);
}

bool 			hashtbl_get(t_hashtbl *tbl, uint32_t key, void **hook)
{
	uint16_t	i;
	bool		found;
	uint16_t	hash;
	uint16_t	init_inx;
	uint16_t	cur_inx;
	uint16_t	probe_dist;

	i = 0;
	probe_dist = 0;
	found = false;
	hash = jenkins_hash((uint8_t *)&key, sizeof(uint32_t));
	init_inx = hash % tbl->bkt_cnt;
	while (i < tbl->prb_max)
	{
		cur_inx = (init_inx + i) % tbl->bkt_cnt;
		fill_distance_to_initinx(tbl, cur_inx, &probe_dist);
		if (tbl->buckets[cur_inx].data == NULL || i > probe_dist)
			break;
		if (tbl->buckets[cur_inx].key == key)
		{
			*hook = tbl->buckets[cur_inx].data;
			found = true;
			break;
		}
		i++;
	}
	return (found);
}
