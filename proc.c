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

#include <net/if.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <config.h>
#include <typedef.h>
#include <macro.h>

static char buffer[10240];
static char ifname[IFNAMSIZ];

char *get_ifname()  __attribute__ ((deprecated));

char *
get_ifname() 
{
	static FILE *proc_net_dev;
	int i=0;

	if (!proc_net_dev) {	
		proc_net_dev= fopen(PROC_NET_DEV,"r");
		if (proc_net_dev == NULL)
			fatal("fopen()");

		fgets(buffer, sizeof(buffer),proc_net_dev);
		fgets(buffer, sizeof(buffer),proc_net_dev);
	}

	if ( fgets(buffer, sizeof(buffer),proc_net_dev) ) {
		sscanf(buffer,"%[^:]",ifname); 
		while (ifname[i]==' ') 
			i++;
		return (ifname+i);
	}	
		
	return (NULL);
}	



char *
get_ifname_new()
{
        static struct ifreq *ifr, *iflast, ifreq_io;
        struct ifconf ifc; 
        struct sockaddr_in *ptr;
        static int sd;

        /* dummy dgram socket for ioctl */

	if ( sd != 0 )	
		goto in_cycle;

        if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
                fatal("socket()");


        ifc.ifc_buf = buffer;
        ifc.ifc_len = sizeof(buffer);

        /* getting ifs: this fills ifconf structure. */
        if (ioctl(sd, SIOCGIFCONF, &ifc) < 0)
                fatal("SIOCGIFCONF");

        close(sd);

        /* line_up ifreq structure */
        ifr    = (struct ifreq *) buffer;
        iflast = (struct ifreq *) ((char *) buffer + ifc.ifc_len);

        if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
                fatal("socket()");

 in_cycle:

        for (; ifr < iflast; )
        {
		char *ret;
                ptr = (struct sockaddr_in *) & ifr->ifr_addr;
                memcpy(&ifreq_io, ifr, sizeof(ifr->ifr_name) + sizeof(struct sockaddr_in));

                if (ioctl(sd, SIOCGIFFLAGS, &ifreq_io) < 0) 
                        fatal("SIOCGIFFLAGS");

		ret = ifr->ifr_name;
		ifr = (struct ifreq *)( (void *)ifr+ sizeof(ifr->ifr_name)+ sizeof(struct sockaddr_in) );
		return (ret);
	}

	return (NULL);
}




static char 	*a_buff;
static int   	a_buff_len;

stats_t *
get_stats(char *ifn)
{
	FILE *proc_net_dev; stats_t *ret;
	char *r;
	int i=0;

	proc_net_dev= fopen(PROC_NET_DEV,"r");
        if (proc_net_dev == NULL)
        	fatal("fopen()");

	ret = (stats_t *)calloc(sizeof(stats_t),1);
	if (ret == NULL)
		fatal("calloc()");

	/* skip 2 lines */
	getline(&a_buff, &a_buff_len, proc_net_dev);
	getline(&a_buff, &a_buff_len, proc_net_dev);
	
        while ( (getline(&a_buff, &a_buff_len, proc_net_dev) ) != EOF ) {

                sscanf(a_buff,"%[^:]",ifname);

		if (strstr(ifname,ifn) == NULL)
			continue;

		r = strchr(a_buff,':');
		if (r == NULL)
			fatal(__INTERNAL__);
		
		sscanf(r+1, "%*lu %lu %lu %lu %lu %lu %*lu %*lu %*lu %lu %lu %lu %lu %lu", 
			&ret->rx_packets,
			&ret->rx_errs,
			&ret->rx_drop,
			&ret->rx_fifo,
			&ret->rx_frame,
			&ret->tx_packets,
			&ret->tx_errs,
			&ret->tx_drop,
			&ret->tx_fifo,
			&ret->tx_colls);
		return (ret);
        }
	
	return (NULL);
}
