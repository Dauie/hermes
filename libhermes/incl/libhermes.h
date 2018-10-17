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
# include <pthread.h>
# include "error_codes.h"

/*
**	Structs
*/

/*
** Wrapper Struct for different data structures
*/
typedef union			s_swis
{
	struct s_swislist 	*slist;
	struct s_swistree 	*stree;
}						t_swis;

typedef struct		s_swistree
{
	size_t 			size;
	struct s_node	*root;
	struct s_node	*min;
	struct s_node	*max;
	void			*data;
}					t_tree;

typedef struct 		s_swislist
{
	size_t 			size;
	struct s_item	*start;
	struct s_item	*end;
	struct s_item	*min;
	struct s_item	*max;
	void			*data; /* auxiliary data */
}					t_deque;

typedef struct		s_node
{
	struct s_node	*left;
	struct s_node	*right;
	void			*data;
}					t_node;

typedef struct		s_item
{
	struct s_item	*prev;
	struct s_item	*next;
	void			*data;
}					t_item;

typedef struct 			s_thread
{
	uint16_t 			id;
	pthread_t 			thread;
	uint8_t 			alive: 1;
	uint8_t 			working: 1;
}						t_thread;

typedef struct 			s_thrpool
{
	uint16_t 			thr_count;
	t_thread			**threads;
}						t_thrpool;

/*
**	Integer helper functions
*/
void				swap_uint8(uint8_t *l, uint8_t *r);
void				swap_uint16(uint16_t *l, uint16_t *r);
void				swap_uint32(uint32_t *l, uint32_t *r);
void				swap_uint64(uint64_t *l, uint64_t *r);

/*
**	Character Manipulation Functions
*/
char				*itoa(int n);
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
t_node * new_node(void **data);
void				del_node(t_node **node, bool deldata);

/*
**	BST Functions
*/
t_deque				*tree_to_deque(t_tree **tree);
bool				add_node_bst(t_node **tree, void **data, int (*cmp)(void *, void *));
bool				remove_node_bst(t_node **node, void *key, int (*cmp)(void *, void *),
									void *(*min)(t_node *));
t_node				*bst_search(t_node **tree, void *data, int (*cmp)(void *, void *));
t_node				*tree_search(t_node **tree, void *data, int (*cmp)(void *, void *));

/*
**	List Functions
*/
void				del_list(t_deque **list, bool deldata);
//bool				add_list_end(t_list **list, void **node);
//bool				add_list_head(t_list **list, void **data);
//bool				remove_node_list(t_list **node, bool deldata);
//bool				remove_list_head(t_list **list, bool deldata);
t_item				*pop(t_deque **deq);
void				*peek(t_deque **list);
bool				enqueue(t_deque **list, void **data);
bool				push(t_deque **list, void **data);
t_deque				*new_deque(void);

/*
**	Error Management Functions
*/
int					hermes_error(int errcode, int str_amt, ...);

#endif //hermes_hermes_H
