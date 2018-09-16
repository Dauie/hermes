t_node *construct_node(void *data, size_t data_len) {
	size_t	i;
	void	*node;
	char	*bytes;
	size_t	node_len;

	if (data == NULL)
		return (NULL);
	node_len = sizeof(t_node);
	node = (t_node*)memalloc(node_len);
	node->data = (data)memalloc(data_len);

	i = node_len - data_len - 1;
	bytes = (char*)data;
	while (++i < node_len) {
		memcpy(node->data[i], bytes[i], sizeof(bytes[i]));
	}
	return (node);
}