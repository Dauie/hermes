
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
typedef struct			s_node
{
	struct s_node		*left;
	struct s_node		*right;
	void				*data;
}						t_node;


/*
**	Character Manipulation Functions
*/
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
int						add_node(t_node **tree, t_node **node,
									int (*cmp)(void *, void *));
int						remove_node(t_node **root, void *key, int (*cmp)(void *, void *), void *(*min)(t_node *));
t_node					*bst_search(t_node **tree, void *data, int (*cmp)(void *, void *));
t_node					*new_node(void);


/*
**	Error Management Functions
*/
int						hermes_error(int errcode, int fatal, int str_amt, ...);

#endif //hermes_hermes_H
