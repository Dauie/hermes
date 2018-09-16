#include "../../incl/parser.h"
#include "../../incl/defined.h"

int verify_ip(uint32_t *ptr, char *ip) {
	if (!inet_pton(AF_INET, ptr, ip))
		return (-1);
	if (*ptr <= 0 || *ptr >= IP_MAX)
		return (-1);
	return (0);
}

t_ip4 *set_ip4(uint32_t ip) {
	t_ip4 *data;

	if (!ip)
		return (NULL);
	if (!(data = (t_ip4*)memalloc(sizeof(t_ip4))))
		return (NULL);
	data->addr = ip;
	return (data);
}

uint32_t get_ip(char * ip_str)
{
    /* construct_ip() takes two parameters:
     *
     *  @p ip is the 2D char array with 1 or more
     *      IPs
     *
     * -------------------------------------------
     *
     *  ip_a = start, ip_z = end
     *
     *  if end < start  : return an error message
     *  if start < end  : create an ip range struct
     *  if start == end : create a singleton ip struct
     */

    uint32_t ip_addr;

	if (ip_str == NULL)
		return ((uint32_t)NULL);
	/* while looping through
	 * ip addresses
	 */
	if (verify_ip(&ip_addr, ip_str))
		return (ip_addr);
    return ((uint32_t)NULL);
}

int parse_ip(t_targetlist *ip_list, char *args)
{
    /* h_ip() takes two parameters:
     *  @p workload is a pointer to the
     *      main job struct
     *
     *  @p args is a single IP to be
     *      verified and sorted in the
     *      IPlist
     *
     * --------------------------------
     *
     */

	uint32_t 	ip;

    if (args == NULL)
    	return (FAILURE);
	if (!(ip = set_ip4(get_ip(args))))
		return (FAILURE);
	listadd_head(ip_list, ip);
    return (SUCCESS);
}