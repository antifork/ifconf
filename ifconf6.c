#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#define PROC_IFINET6	"/proc/net/if_inet6"

#define IPV6_ADDR_ANY           0x0000U
#define IPV6_ADDR_UNICAST       0x0001U
#define IPV6_ADDR_MULTICAST     0x0002U
#define IPV6_ADDR_ANYCAST       0x0004U
#define IPV6_ADDR_LOOPBACK      0x0010U
#define IPV6_ADDR_LINKLOCAL     0x0020U
#define IPV6_ADDR_SITELOCAL     0x0040U
#define IPV6_ADDR_COMPATv4      0x0080U
#define IPV6_ADDR_SCOPE_MASK    0x00f0U
#define IPV6_ADDR_MAPPED        0x1000U
#define IPV6_ADDR_RESERVED      0x2000U         /* reserved address space */


void
inet6_print_addr(char *ifn)
{
	FILE *f;
	char addr6p[8][5], devname[20], addr6[40];
	struct sockaddr_in6 sap;
	struct in6_addr in_addr6;
    	int plen, scope, dad_status, if_idx;

	if ( (f=fopen(PROC_IFINET6,"r")) == NULL)  
		/* not ipv6 compliant */
		return;

	while (fscanf(f, "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %20s\n",
                      	addr6p[0], addr6p[1], addr6p[2], addr6p[3],
                      	addr6p[4], addr6p[5], addr6p[6], addr6p[7],
                  	&if_idx, &plen, &scope, &dad_status, devname) != EOF) {
		
		if ( strcmp(ifn,devname) )
			continue;

                sprintf(addr6, "%s:%s:%s:%s:%s:%s:%s:%s",
                        addr6p[0], addr6p[1], addr6p[2], addr6p[3],
                        addr6p[4], addr6p[5], addr6p[6], addr6p[7]);

		/* pretty host */
		inet_pton(PF_INET6,addr6,&in_addr6);
		inet_ntop(PF_INET6,&in_addr6, addr6, 40);

		printf("\tinet6 addr: %s/%d Scope:",addr6,plen);

                switch (scope) {
                case 0:
                    printf("Global");
                    break;
                case IPV6_ADDR_LINKLOCAL:
                    printf("Link");
                    break;
                case IPV6_ADDR_SITELOCAL:
                    printf("Site");
                    break;
                case IPV6_ADDR_COMPATv4:
                    printf("Compat");
                    break;
                case IPV6_ADDR_LOOPBACK:
                    printf("Host");
                    break;
                default:
                    printf("Unknown");
                }

		printf("\n");
	}
	
	close (f);
	return ;
}
