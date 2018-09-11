
int populate_ip(t_job * workload, uint32_t ip)
{
    return (0);
}

int construct_ip(t_job * workload, uint32_t ** ip)
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
    char **     ip_r;
    uint32_t    ip_a;
    uint32_t    ip_z;

    if (ip == NULL) return (-1);

    ip_a = 0;
    ip_z = 0;
    /* while looping through
     * ip addresses
     */
    for (i = 0; ip[i]; i++)
    {
        /* while looping through
         * ip octets
         */
        for (j = 0; ip[i][j]; j++)
        {
            /* if ip is in accepted range */
            if (ip[i][j] >= 0 || ip[i][j] <= 255)
                /* shift the LSB over by one
                 * octet and logically OR them
                 * with the starting IP
                 */
                ip_a = ip[i][j] | (ip_a << 8);
            else
                return (-1);
        }

        /* swap the two
         * IP addrs so
         * we can use
         * ip_a on the
         * second loop
         */
        (ip_a ^= ip_z),
        (ip_z ^= ip_a),
        (ip_a ^= ip_z);

        /* we need both IPs before
         * checks
         */
        if (ip_a == 0);
        else if (ip_z < ip_a)
            //FAILURE
            //IP_Z !> IP_A
            return (-1);
        else if (ip_a < ip_z)
        {
            // TODO : populate_ip() with IP range struct
            /*
            if (populate_ip(workload, ip_a) < 0)
                return (0);
             */
        }
        else
            // TODO : populate_ip() with single IP struct
            // populate_ip()
        /* START DEBUG */
        char *test;
        test = inet_pton(AF_INET, ip_a, test, INET_ADDRSTRLEN);
        printf("TESTING IP CONVERSION == %s\n", test);
        /* END DEBUG */
    }

    if (ip_r) free(ip_r);

    return (0);
}

uint32_t ** split_range(char * ips)
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
    int         len;
    char **     range;
    /* IP Range */
    uint32_t ** ip_r;

    len(strlen(ips));
    if (memchr('.', ips, len))
        if ((q = ft_strplit(ips, '.')) == NULL)
            return (NULL);
    else
        return (NULL);

    /* create a 2 * 4 sized table to hold up to two IPs*/
    ip_r = (uint32_t**)calloc(2, sizeof(uint32_t * 4));
    if (ip_r == NULL)
        return (NULL);

    for (i = 0; q[i]; i++)
    {
        /* if range */
        if (memchr('-', ips, len))
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
            if (inet_pton(AF_INET, range[0], ip_r[0][i]) == NULL ||
                inet_pton(AF_INET, range[1], ip_r[1][i]) == NULL)
                return (NULL)
        }
        /* if not range */
        else
        {
            /* both rows are the same IP */
            if (inet_pton(AF_INET, q[i], ip_r[0][i]) == NULL ||
                inet_pton(AF_INET, q[i], ip_r[1][i]) == NULL)
                return (NULL)
        }
    }

    if (q) free(q);
    if (range) free(range);
    if (i != 4) return (NULL);

    return (ip_r);
}

int h_ip(t_job * workload, char * args)
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

    int         i;
    char **     ips;
    char *      ip;
    int         subnet;

    if (args == NULL) return (1);

    if (memchr('/', args, strlen(args)))
    {
        ips = ft_strsplit(args, '/');
        ip = ips[0];
        subnet = ips[1];
    }
    else
    {
        ip = args;
    }

    if (construct_ip(workload, split_range(ip)) < 0)
            return (-1);
    return (0);
}