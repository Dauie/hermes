//
// Created by Samuel Collet on 10/12/18.
//

# include <stdio.h>
# include <arpa/inet.h>
# include <netinet/ip.h>
# include <netinet/in.h>
# include <netdb.h>
# include <netinet/ip_icmp.h>

int main(void)
{
	uint32_t 	size;
	char 		ip1_s[20];
	char		ip2_s[20];
	uint32_t	ip1_n = htonl(169345025); //192.168.12.10
	uint32_t	ip2_n = htonl(3237937164); //192.255.0.12
	uint32_t	ip1_h = 169345025;
	uint32_t	ip2_h = 3237937164;


	inet_ntop(AF_INET, &ip1_n, ip1_s, 20);
	inet_ntop(AF_INET, &ip2_n, ip2_s, 20);
	printf("ipaddrs\n%s\n%s\n", ip1_s, ip2_s);
	size = (ntohl(ip2_n) - ntohl(ip1_n)) / 4;
	printf("size\n%d\n\n", size);
	while (ip1_h < ip2_h)
	{
		ip1_n = htonl(ip1_h); //192.168.12.10
		ip2_n = htonl(ip2_h); //192.255.0.12
		inet_ntop(AF_INET, &ip1_n, ip1_s, 20);
		inet_ntop(AF_INET, &ip2_n, ip2_s, 20);
		printf("ipaddrs\n%s\n%s\n", ip1_s, ip2_s);
		ip1_h += size;
	}
	return (0);
}