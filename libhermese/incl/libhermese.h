
#ifndef LIB_HERMESE_H
# define LIB_HERMESE_H

# include <memory.h>
# include <stdint.h>
# include <stdlib.h>

# ifndef fast_swap_ints
# define fast_swap_ints(x, y) ((x ^= y),(y ^= x),(x ^= y))
# endif

#ifndef determine_type

/*
**	Structs
*/
typedef struct			s_node
{
	struct s_node		*next;
	void				*data;
}						t_node;

/*TODO MAKE ALL THESE v*/
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


/*
**	Error Management Functions
*/
void				hermese_error();

#endif //HERMESE_HERMESE_H
