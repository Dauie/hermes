#ifndef LIB_HERMESE_H
# define LIB_HERMESE_H

# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <stddef.h>
# include <string.h>
# include <sys/time.h>

# include <arpa/inet.h>
# include <netinet/ip.h>
# include <netinet/in.h>

#ifndef determine_type
#endif
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
**	Node Operation Function
*/
t_node				*new_node(void);
void				set_node(t_node *node, void *data, size_t size);


/*
**	Character Manipulation Functions
*/

char				*strtrim(const char *s);
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
