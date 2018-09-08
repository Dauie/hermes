
int populate_ip()
{

}

int verify_range()
{

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
    char **     ip;
    char *      ip_n;
    int         subnet;

    if (args == NULL) return (1);

    if (memchr('/', args, strlen(args)))
    {
        ip = ft_strsplit(args, '/');
        ip_n = ip[0];
    }
    else
    {
        ip_n = args;
    }

    if (populate_ip() < 0)
        //inet_pton(AF_INET, ip_n, &ip_n) < 0)
    {
        if (verify_range() < 0)
            return (-1);
        else
            split_range();
    }

    return (0);
}