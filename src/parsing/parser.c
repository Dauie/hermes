
char ** parse_opts(char * args)
{
	/* strtok by delim(-)
	 * recieve array of tuples
	 * 
	 */
	return(ft_strsplit(args, '-'));
}

t_manager * parser(t_manager workload, char * args) // TODO : args
{
	/* each valid opt will have a
	 * 	corresponding handler for
	 * 	the argument provided
	 * 	inside of our dispatch table
	 */

	char ** opts;

	opts = parse_opts(args);
	if (!opts)
		help(workload, NULL);
	while (*opts)
	{
		handle(workload, *opts)
		if (*opts)
			free(*opts);
		++opts;
	}
	return (workload);
}
