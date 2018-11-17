#include "../incl/libhermes.h"

int			fill_distance_to_initinx(t_hashtbl *tbl, uint16_t inx_stored, uint16_t *distance)
{
	uint16_t index_init;

	if(tbl->buckets[inx_stored].data == NULL)
		return FAILURE;
	index_init = tbl->buckets[inx_stored].hash % tbl->bkt_cnt;
	if (index_init <= inx_stored) {
		*distance = inx_stored - index_init;
	} else {
		*distance = inx_stored + (tbl->bkt_cnt - index_init);
	}
	return SUCCESS;
}

uint32_t	murmur3_32(const uint8_t *key, size_t len, uint32_t seed)
{
	size_t			i;
	uint32_t		h;
	uint32_t		k;
	const uint32_t	*key_x4;

	h = seed;
	if (len > 3) {
		key_x4 = (const uint32_t*) key;
		size_t i = len >> 2;
		do {
			k = *key_x4++;
			k *= 0xcc9e2d51;
			k = (k << 15) | (k >> 17);
			k *= 0x1b873593;
			h ^= k;
			h = (h << 13) | (h >> 19);
			h = (h * 5) + 0xe6546b64;
		} while (--i);
		key = (const uint8_t*) key_x4;
	}
	if (len & 3) {
		i = len & 3;
		k = 0;
		key = &key[i - 1];
		do {
			k <<= 8;
			k |= *key--;
		} while (--i);
		k *= 0xcc9e2d51;
		k = (k << 15) | (k >> 17);
		k *= 0x1b873593;
		h ^= k;
	}
	h ^= len;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return (h);// % len;
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
	if (!(ht->buckets = memalloc(sizeof(t_hashbkt) * size)))
		return (NULL);
	ht->bkt_cnt = (uint16_t)size;
	ht->prb_max = (uint16_t)size;
	return (ht);
}

bool 			hashtbl_add(t_hashtbl *tbl, uint8_t *key, uint32_t ksz, void **value)
{
	uint16_t	i;
	uint16_t	hash;
	uint16_t	init_inx;
	uint16_t	cur_inx;
	uint16_t	prb_dist;
	uint16_t	prb_cur;
	uint16_t	hash_temp;
	void		*entry;
	void		*data_tmp;
	int			num_swaps;


	if (tbl->bkt_usd == tbl->bkt_cnt)
		return (false);
	tbl->bkt_usd += 1;
	hash = murmur3_32(key, ksz, 42);
	init_inx = hash % tbl->bkt_cnt;
	prb_dist = 0;
	prb_cur = 0;
	entry = *value;
	num_swaps = 0;
	for (i = 0; i < tbl->prb_max; i++)
	{
		cur_inx = (init_inx + i) % tbl->bkt_cnt;
		if (tbl->buckets[cur_inx].data == NULL)
		{
			tbl->buckets[cur_inx].data = entry;
			tbl->buckets[cur_inx].hash = hash;
			break;
		}
		else
		{
			fill_distance_to_initinx(tbl, cur_inx, &prb_dist);
			if (prb_cur > prb_dist)
			{
				data_tmp = tbl->buckets[cur_inx].data;
				hash_temp = tbl->buckets[cur_inx].hash;
				tbl->buckets[cur_inx].data = entry;
				tbl->buckets[cur_inx].hash = hash;
				entry = data_tmp;
				hash = hash_temp;
				prb_cur = prb_dist;
				num_swaps += 1;
			}
		}
		prb_cur++;
	}
	return (true);
}

bool			hashtbl_rm(t_hashtbl *tbl, uint8_t *key, uint16_t keysz)
{
	uint16_t hash;
	uint16_t index_init;
	bool found;
	uint16_t cur_inx;
	uint16_t prb_dist;
	uint16_t prev_inx;
	uint16_t swap_inx;
	uint16_t i;

	found = false;
	hash = murmur3_32(key, keysz, 42);
	init_inx = hash % tbl->bkt_cnt;
	cur_inx = 0;
	prb_dist = 0;
	for (i = 0; i < tbl->bkt_cnt; i++)
	{
		cur_inx = (uint16_t)(index_init + i) % tbl->bkt_cnt;
		fill_distance_to_initinx(tbl, cur_inx, &prb_dist);
		if (tbl->buckets[cur_inx].data == NULL || i > prb_dist) {
			break;
		}
		if (memcmp(tbl->buckets[cur_inx].data, key, keysz) == 0) {
			found = true;
			break;
		}
	}
	if (found)
	{
		tbl->buckets[cur_inx].data = NULL;
		tbl->buckets[cur_inx].hash = 0;
		for (i = 1; i < tbl->bkt_cnt; i++)
		{
			prev_inx = (uint16_t)(cur_inx + i - 1) % tbl->bkt_cnt;
			swap_inx = (cur_inx + i) % tbl->bkt_cnt;
			if (tbl->buckets[swap_inx].data == NULL)
			{
				tbl->buckets[prev_inx].data = NULL;
				break;
			}
			if (fill_distance_to_initinx(tbl, swap_inx, &probe_dist) != SUCCESS)
			{
				hermes_error(FAILURE, "fill_distance_to_initinx()");
			}
			if (prb_dist == 0)
			{
				tbl->buckets[prev_inx].data = NULL;
				break;
			}
			tbl->buckets[prev_inx].data = tbl->buckets[swap_inx].data;
			tbl->buckets[prev_inx].hash = tbl->buckets[swap_inx].hash;
		}
		tbl->bkt_usd -= 1;
		return (true);
	}
	else
		printf("key not found == ");
	return (false);
}

bool			hashtbl_get(t_hashtbl *tbl, uint8_t *key, uint16_t keysz, void **hook)
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
	hash = murmur3_32(key, keysz, 42);
	init_inx = hash % tbl->bkt_cnt;
	while (i < tbl->prb_max)
	{
		cur_inx = (init_inx + i) % tbl->bkt_cnt;
		fill_distance_to_initinx(tbl, cur_inx, &probe_dist);
		if (tbl->buckets[cur_inx].data == NULL || i > probe_dist)
			break;
		if (memcmp(tbl->buckets[cur_inx].data, key, keysz) == 0)
		{
			*hook = tbl->buckets[cur_inx].data;
			found = true;
			break;
		}
		i++;
	}
	return (found);
}
