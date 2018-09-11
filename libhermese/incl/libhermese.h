
#ifndef LIB_HERMESE_H
# define LIB_HERMESE_H

# include <memory.h>
# include <stdint.h>
# include <stdlib.h>

typedef struct				s_node
{
	void					*data;
	struct s_node			*next;
}							t_node;

/*
**	Node Functions
*/
t_node						*make_node(void *data, size_t datalen);
void						listadd_end(t_node **list, t_node *node);
void						listadd_head(t_node **list, t_node *node);
void						listdel(t_node **list, void (*del)(void*, size_t));

/*
**	Memory Helper Functions
*/
void						*memalloc(size_t size);

#endif //HERMESE_HERMESE_H
