
#ifndef LIB_HERMESE_H
# define LIB_HERMESE_H

# include <memory.h>
# include <stdint.h>
# include <stdlib.h>

/*
**	Structs
*/
typedef struct		s_node
{
	void			*data;
	struct s_node	*next;
}					t_node;

/*TODO MAKE ALL THESE v*/
/*
**	Character Manipulation Functions
*/
char				**strsplit(const char *str, char delim);
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


/*
**	Error Management Functions
*/
void				hermese_error();

#endif //HERMESE_HERMESE_H