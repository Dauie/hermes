
#ifndef LIB_HERMES_H
# define LIB_HERMES_H

# include <sys/errno.h>
# include <memory.h>
# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>

# include "error_codes.h"
# include "bool.h"

/*
**	Structs
*/
typedef struct			s_bst
{
	struct s_bst		*left;
	struct s_bst		*right;
	void				*data;
}						t_bst;

typedef struct			s_lst
{
	struct s_lst		*next;
	struct s_lst		*prev;
	void				*data;
}						t_lst;

/*
**	Integer helper functions
*/
void					swap_uint8(uint8_t *l, uint8_t *r);
void					swap_uint16(uint16_t *l, uint16_t *r);
void					swap_uint32(uint32_t *l, uint32_t *r);
void					swap_uint64(uint64_t *l, uint64_t *r);

/*
**	Character Manipulation Functions
*/
char                    *itoa(int n);
size_t					cntwrds(char *str, char c);
char					**strsplit(const char *str, char delim);
char					*strsub(char const *s, int start, size_t len);
void					tbldel(char ***tbl);

/*
**	Memory Helper Functions
*/
void					*memalloc(size_t size);

/*
**	BST Functions
*/
int						add_node_bst(t_bst **tree, void **data, int (*cmp)(void *, void *));
int						remove_node_bst(t_bst **tree, void *key, int (*cmp)(void *, void *), void *(*min)(t_bst *));
t_bst					*bst_search(t_bst **tree, void *data, int (*cmp)(void *, void *));
t_bst					*tree_search(t_bst **tree, void *data, int (*cmp)(void *, void *));
t_bst					*new_node_bst(void);

/*
**	List Functions
*/
void					add_node_list_end(t_lst **list, t_lst *node);
void					add_node_list_head(t_lst **list, t_lst *node);
void					del_list_node(t_lst **node);
t_lst					*new_node_list(void);

/*
**	Error Management Functions
*/
int						hermes_error(int errcode, int fatal, int str_amt, ...);

#endif //hermes_hermes_H
