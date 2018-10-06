#ifndef LIB_HERMES_H
# define LIB_HERMES_H

# include <sys/errno.h>
# include <unistd.h>
# include <memory.h>
# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>

# include "error_codes.h"
# include "bool.h"

/*
**	Structs
*/
typedef struct		s_node
{
	struct s_node	*left;
	struct s_node	*right;
	void			*data;
}					t_node;


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
t_node				*new_node(void **data);
void				del_node(t_node **node);

/*
**	BST Functions
*/
int					add_node_bst(t_node **tree, void **data, int (*cmp)(void *, void *));
int					remove_node_bst(t_node **tree, void *key, int (*cmp)(void *, void *), void *(*min)(t_node *));
t_node				*bst_search(t_node **tree, void *data, int (*cmp)(void *, void *));
t_node				*tree_search(t_node **tree, void *data, int (*cmp)(void *, void *));

/*
**	List Functions
*/
void                   del_list(t_node **list);
void				add_node_list_end(t_node **list, void **node);
void				add_node_list_head(t_node **list, void **data);
void				remove_node_list(t_node **node);
void				remove_node_list_head(t_node **list);

/*
**	Error Management Functions
*/
int					hermes_error(int errcode, int fatal, int str_amt, ...);

#endif //hermes_hermes_H
