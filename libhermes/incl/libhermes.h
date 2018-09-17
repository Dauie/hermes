
#ifndef LIB_HERMES_H
# define LIB_HERMES_H

# include <zconf.h>
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
	struct s_node		*next;
	void				*data;
}						t_node;


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
**	List Functions
*/
void				listadd_end(t_node **list, t_node *node);
void				listadd_head(t_node **list, t_node *node);
t_node				*new_node(void);
void				set_node(t_node *node, void *data, size_t size);


/*
**	Error Management Functions
*/
int				hermes_error(int errcode, int fatal, int str_amt, ...);

#endif //hermes_hermes_H
