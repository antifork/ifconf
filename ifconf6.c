/*
 *  $Id$
 *
 *  Copyright (c) 2004 Bonelli Nicola <awgn@antifork.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#include <macro.h>

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
