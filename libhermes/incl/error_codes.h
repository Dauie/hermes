#ifndef ERROR_CODES_H
#define ERROR_CODES_H

typedef enum	s_ret_codes
{
	ERR_DISCON = -2,
	FAILURE = -1,
	SUCCESS = 0,
}				t_ret_codes;

# define PTR_FAILURE ((void*)-1)

# define TEXIT_FAILURE (2)

#endif //HERMES_ERROR_CODES_H
