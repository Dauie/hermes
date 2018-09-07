#include ""

t_manager help(t_manager * job, char * arg)
{
	printf("%d\n", help_menu);
	return (job);
}

t_manager handle_ports(t_manager * job, char * arg)
{

	return (job);
}

t_manager handle_ip(t_manager * job, char * arg)
{	
	/* for ip in list of IPs where {
	 * 		1 or more IPs are
	 * 		provided
	 * 	}:
	 * 	analyze IP by delim(.)
	 * 	and check range
	 *	if not good range:
	 *		job.error = 1
	 *	else:
	 *		dependent on
	 *		number of
	 *		workers ...
	 *
	 *		job.iplist.
	 *		append(IP)
	 */

	return (job);
}

t_manager handle_file(t_manager * job, char * arg)
{
	/* parse file using standardized
	 * file format and handle any file
	 * args like CLI args as well as
	 * IP lists and port lists
	 */

	return (job);
}

t_manager handle_speedup(t_manager * job, char * arg)
{
	
	return (job);
}

t_manager handle_scan(t_manager * job, char * arg)
{
	/* parse list of scans from string
	 * by delim( ) and verify each scan
	 * for scan in scans:
	 * 	job.scanlist.append(scan)
	 */
	return (job);
}

t_manager * handle(t_manager * workload, char * opt_arg)
{
	int 	i;
	int 	len;
	char ** opt;
	char ** arg;
	char * 	next;
	char * 	result;

	len = strlen(opt_arg);
	next = (char *)memccpy(opt, opt_arg, ' ', len);
	len -= strlen(opt);
	// TODO : chk next
	memccpy(arg, opt_arg + next, '\0', len);

	i = -1;
	result = NULL;
	while (++i < DTAB_ENTRIES && result == NULL)
		result = strstr(opt, DTAB[i].name);

	if (result != NULL)
		DTAB[i].function(workload, arg);

	return (workload);
}
