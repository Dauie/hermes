#include "../incl/libhermese.h"

t_node		*construct_node(void *data, size_t data_len)
{
	size_t	i;
	void	*node;
	char	*bytes;

	if (data == NULL)
		return (NULL);
	node = (t_node*)memalloc(sizeof(t_node));
	node->data = (data)memalloc(data_len);

	i = node_len - data_len - 1;
	bytes = (char*)data;
	while (++i < node_len) {
		memcpy(((t_node*)node)->data[i], bytes[i], sizeof(bytes[i]));
	}
	return (node);
}