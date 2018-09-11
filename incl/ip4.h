#ifndef HERMESE_IP4_H
# define HERMESE_IP4_H

typedef struct		s_ip4range
{
	u_int32_t		start;
	u_int32_t		end;
}					t_ip4range;

typedef struct			s_ip4_node
{
	u_int32_t			addr;
	struct s_ip4_node	*next;
}						t_ip4node;

typedef struct				s_ip4range_node
{
	t_ip4range				range;
	struct s_ip4range_node	*next;
}							s_ip4range_node;

#endif