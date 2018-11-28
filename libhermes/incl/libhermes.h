#ifndef LIB_HERMES_H
# define LIB_HERMES_H

# include <sys/errno.h>
# include <unistd.h>
# include <memory.h>
# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
# include <stdarg.h>
# include <stdbool.h>
# include <limits.h>

# include "error_codes.h"

/*
**	Structs
*/

typedef struct		s_node
{
	struct s_node	*left;
	struct s_node	*right;
	void			*data;
}					t_node;

typedef struct		s_hashbkt
{
	uint16_t		hash;
	uint32_t		key;
	void			*data;
}					t_hashbkt;

typedef struct		s_hashtbl
{
	uint16_t		prb_max;
	uint16_t		bkt_cnt;
	uint16_t		bkt_usd;
	t_hashbkt		*buckets;
}					t_hashtbl;

/*
**	Integer helper functions
*/
unsigned int		pow2_round(unsigned int nb);
void				swap_uint8(uint8_t *l, uint8_t *r);
void				swap_uint16(uint16_t *l, uint16_t *r);
void				swap_uint32(uint32_t *l, uint32_t *r);
void				swap_uint64(uint64_t *l, uint64_t *r);

/*
**	Character Manipulation Functions
*/
size_t				cntwrds(char *str, char c);
char				**strsplit(const char *str, char delim);
char				*strsub(char const *s, int start, size_t len);
void				tbldel(char ***tbl);

/*
**	Memory Helper Functions
*/
void				*memalloc(size_t size);

/*
**	Node helper functions
*/
t_node				*new_node(void **data);
void				del_node(t_node **node, bool deldata);

/*
**	BST Functions
*/
void 				del_tree(t_node **tree, bool deldata);
bool				add_node_bst(t_node **tree, void **data, int (*cmp)(void *, void *));
bool				rm_node_bst(t_node **tree, void *key, int (*cmp)(void *, void *), void *(*min)(t_node *));
t_node				*bst_search(t_node **tree, void *data, int (*cmp)(void *, void *));
t_node				*tree_search(t_node **tree, void *data,
								   int (*cmp)(void *, void *));

t_node				*bst_to_clist(t_node **tree);

/*
**	List Functions
*/
bool				list_add_head(t_node **list, void **data);
bool				list_add_tail(t_node **list, void **data);
bool				clist_add_inorder(t_node **clist, void **data);

bool				list_rm_node(t_node **list, t_node **rm, bool deldata);
bool				clist_rm_tail(t_node **clist, bool deldata);
bool				clist_rm(t_node **clist, void *data, int (*cmp)(void *, void *));
void				del_list(t_node **clist, bool deldata);
bool				rm_node(t_node **node, bool deldata);

/*
**	Hash Functions
*/
void				*new_hashtbl(size_t size);
bool				hashtbl_add(t_hashtbl *tbl, uint32_t key,
								void **value);

bool hashtbl_get(t_hashtbl *tbl, uint32_t key, void **hook);
bool				hashtbl_rm(t_hashtbl *tbl, uint32_t key);

/*
**	Error Management Functions
*/
int					hermes_error(int errcode, char *fmt, ...);

#endif //LIB_HERMES_H
