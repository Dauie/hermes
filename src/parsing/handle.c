#include "../../hermes.h"

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
    if (isin(opt, D&TAB_ENTRIES, workload))
        // opt does not exist
        // print and exit
	return (workload);
}
