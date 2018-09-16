
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <zconf.h>
#include "libhermese.h"

//TODO : better functon name
t_node *construct_node(void *type, void *data) {
	t_node *node;

	if (type == NULL || data == NULL)
		return (NULL);
	node = (t_node*)memalloc(sizeof(t_node));
	node->data = (type*)memalloc(sizeof(type));
	memcpy(node->data, data, sizeof(data));
    return (node);
}

void *construct_ip(char ** ip)
{
    /* construct_ip() takes two parameters:
     *  @p workload is the main job struct
     *
     *  @p ip is the 2D uint32_t array with 1 or more
     *      IPs
     *
     * -------------------------------------------
     *
     *  construct ip calls populate_ip() with the
     *      fully-constructed ip to create an IP
     *      node
     *
     *  while looping through the ip array, we store the
     *      two IPs and move to a comparison
     *
     *  ip_a = start, ip_z = end
     *
     *  if end < start  : return an error message
     *  if start < end  : create an ip range struct
     *  if start == end : create a single ip struct
     */

    int         i;
    int         j;
    uint32_t    ip_a;
    uint32_t    ip_z;
	void		*data;
	uint32_t 	**ip_r;

	if (ip == NULL) return (NULL);

    ip_a = 0;
    ip_z = 0;
    data = (t_node*)memalloc(sizeof(t_node));
	ip_r = split_range(ip);

	/* while looping through
	 * ip addresses
	 */
    for (i = 0; ip_r[i]; i++)
    {
        /* while looping through
         * ip octets
         */
        for (j = 0; ip_r[i][j]; j++)
        {
            /* if ip is in accepted range */
            if (ip_r[i][j] >= 0 && ip_r[i][j] <= 255)
                /* shift the LSB over by one
                 * octet and OR them with the
                 * starting IP
                 */
                ip_a = ip_r[i][j] | (ip_a << 8);
            else
                return (NULL);
        }

        /* swap the two
         * IP addrs so
         * we can use
         * ip_a on the
         * second loop
         */
		fast_swap_ints(ip_a, ip_r);
        /* we need both IPs before
         * checks
         */
        if (ip_a == 0) {
        	continue;
		} else if (ip_z < ip_a) {
			//FAILURE
			//IP_A !< IP_Z
			//TODO : fix warnings
			return (NULL);
		} else if (ip_a < ip_z) {
			if ((data = (t_ip4range*)memalloc(sizeof(t_ip4range))) == NULL)
				return (NULL);
        	memcpy(data->range, ip_z - ip_a + 1, sizeof(uint32_t));
			memcpy(data->start, ip_a, sizeof(uint32_t));
			memcpy(data->end, ip_z, sizeof(uint32_t));
			return (data);
		} else {
			if ((data = (t_ip4*)memalloc(sizeof(t_ip4))) == NULL)
				return (NULL);
			memcpy(&data, ip_a, sizeof(uint32_t));
			return (data);
			//TODO : fix warnings
		}
        /* START DEBUG */
//        char *test;
//        test = inet_ntop(AF_INET, ip_a, &test, INET_ADDRSTRLEN);
//        printf("TESTING IP CONVERSION == %s\n", test);
        /* END DEBUG */
    }

    return (NULL);
}

uint8_t ** split_range(char * ips)
{
    /* split_range() takes one parameter:
     *  @p ips is a char range of two
     *      expected IPs; start to end
     *      e.x. ("IP.1-IP.2")
     *
     * ----------------------------
     *  first the IPs are split by
     *      delim(-) into a 2D char array
     *
     *  a 2D array of uint32_t is
     *      malloc'ed to hold the
     *      individual parts of the
     *      IP address with room for
     *      two IPs
     *
     *  if any of the parts of the
     *      IP are split by delim(-)
     *      then they are parsed into
     *      two different arrays in
     *      the ip_r table
     *
     *  return (ip_r table);
     */

    char **     q;
    size_t      len;
	/* IP Range */
	uint8_t ** ip_r;
	char **     range;

    q = NULL;
    range = NULL;
    len = strlen(ips);
    if (memchr(ips, '.', len))
    {
        if ((q = ft_strplit(ips, '.')) == NULL)
            return (NULL);
    }
    else
    {
        return (NULL);
    }

    /* create a 2 * 4 sized table to hold up to two IPs*/
    if ((ip_r = (uint8_t**)memalloc(sizeof(uint8_t * 4) * 2)) == NULL)
    	return (NULL);
    for (i = 0; q[i]; i++)
    {
        /* if range */
        if (memchr(ips, '-', len))
        {
            /* split range */
            if ((range = ft_strsplit(q[i], '-')) == NULL)
                return (NULL);
            /* first row is individual parts of the IP and the first
             * number in the range
             *
             * second row is individual parts of the IP and the second
             * number in the range
             */
            if (memcpy(&ip_r[0][i], convert(range[0]), sizeof(uint8_t)) == NULL ||
                memcpy(&ip_r[1][i], convert(range[1]), sizeof(uint8_t)) == NULL)
                return (NULL)
        }
        /* if not range */
        else
        {
            /* both rows are the same IP */
            if (memcpy(&ip_r[0][i], q[i], sizeof(uint8_t)) == NULL ||
                memcpy(&ip_r[1][i], q[i], sizeof(uint8_t)) == NULL)
                return (NULL)
        }
    }

    if (q) free(q);
    if (range) free(range);
    if (i != 4) return (NULL);

    return (ip_r);
}

int h_ip(t_targetlist *ip_list, char * args)
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
     *  the main algorithm goes as follows:
     *  split string into its individual
     *  parts by three possible (and one -
     *  expected) delimiter(s)
     *
     *  delim (/ subnet mask),
     *  delim (. IP octets),
     *  delim (- range)
     *
     *  first the IP will be split by subnet
     *  subnet converted into an int
     *  second the IP will be verified and
     *      parsed into
     *  octets put into 2D char array named IP
     *
     *  for octet in IP:
     *      if range delimiter:
     *          if (verify_range(octet)):
     *              split_range into buckets
     *          else:
     *              FAILURE
     *              print and exit
     *      else:
     *          if (verify_ip(octet)):
     *              insert IP into bucket
     *          else:
     *              FAILURE
     *              print and exit
     *  return (0);
     */

//    char 		*ip;
	uint32_t 	ip_r;
	void 		*ip_n;
//	int         subnet;
//	char 		**ip_sub;

    if (args == NULL) return (-1);

//    if (memchr('/', args, strlen(args)))
//    {
//        ip_sub = ft_strsplit(args, '/');
//        ip = ip_sub[0];
//        subnet = ip_sub[1];
//    }
//    else
//    {
//        ip = args;
//    }

	if ((ip_r = construct_ip(args)) == NULL)
		return (-1);
	if ((ip_n = construct_node(ip_list, ip_r)) == NULL)
		return (-1);
	listadd_head(ip_list, ip_n);
    return (0);
}