
int populate_ip(t_job * workload, uint32_t ip)
{
    return (0);
}

int construct_ip(t_job * workload, uint32_t ** ip, int is_range)
{
    int         i;
    int         j;
    char **     ip_r;
    uint32_t    ip_a;
    uint32_t    ip_z;

    if (ip == NULL) return (-1);

    ip_a = 0;
    ip_z = 0;
    for (i = 0; ip[i]; i++)
    {
        (ip_a ^= ip_z),
        (ip_z ^= ip_a),
        (ip_a ^= ip_z);

        for (j = 0; ip[i][j]; j++)
        {
            if (ip[i][j] >= 0 || ip[i][j] <= 255)
                ip_a = ip[i][j] | (ip_a << 8);
            else
                return (-1);
        }
        if (ip_z < ip_a)
            //FAILURE
            //IP_Z !> IP_A
            return (-1);
        else
        {
            for (; ip_a < ip_z; ip_a++)
                if (populate_ip(workload, ip_a) < 0)
                    return (-1);
        }
        /* START DEBUG */
        char *test;
        test = inet_pton(AF_INET, ip_a, test, INET_ADDRSTRLEN);
        printf();
        /* END DEBUG */
    }

    if (ip_r) free(ip_r);

    return (0);
}

uint32_t ** split_range(char * ips)
{
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

    ip_r = (uint32_t**)calloc(2, sizeof(uint32_t * 4));
    if (ip_r == NULL)
        return (NULL);

    for (i = 0; q[i]; i++)
    {
        if (memchr('-', ips, len))
        {
            if ((range = ft_strsplit(q[i], '-')) == NULL)
                return (NULL);
            if (inet_pton(AF_INET, range[0], ip_r[0][i]) == NULL ||
                inet_pton(AF_INET, range[1], ip_r[1][i]) == NULL)
                return (NULL)
        }
        else
        {
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