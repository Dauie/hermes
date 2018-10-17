#include "../incl/libhermes.h"

t_node 		**new_array(size_t size)
{
	t_node **array;

	if (!size)
		return (NULL);
	array = (t_node**)
}

bool		insert(t_node **array, size_t index, void *data)
{
	t_node *node;

	if (!array || !data)
		return (false);
	node = new_node(data);
	array[index] = node;
	return (true);
}

bool		append(t_node **array, void *data)
{
	size_t len;

	if (!array || !data)
		return (false);
	len = len(array);
}

size_t 		len(t_node **array)
{
	size_t size;

	size = 0;
	while (array[size])
		size++;
	return (size);
}
