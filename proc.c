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

#define PROC_NET_DEV "/proc/net/dev"

#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>

#include <config.h>
#include <typedef.h>

static char buf[512];
static char ifname[IFNAMSIZ];

char *
get_ifname()
{
	static FILE *proc_net_dev;
	int i=0;

	if (!proc_net_dev) {	
		proc_net_dev= fopen(PROC_NET_DEV,"r");
		if (proc_net_dev == NULL)
			fatal("fopen()");

		fgets(buf, sizeof(buf),proc_net_dev);
		fgets(buf, sizeof(buf),proc_net_dev);
	}

	if ( fgets(buf, sizeof(buf),proc_net_dev) ) {
		sscanf(buf,"%[^:]",ifname); 
		while (ifname[i]==' ') 
			i++;
		return (ifname+i);
	}	
		
	return (NULL);
}	


static char *buff;
static int   buff_len;

stats_t
get_stats(char *ifn)
{
	FILE *proc_net_dev;
	stats_t ret;	
	int i=0;

	proc_net_dev= fopen(PROC_NET_DEV,"r");
        if (proc_net_dev == NULL)
        	fatal("fopen()");

	/* skip 2 lines */
	getline(&buff, &buff_len, proc_net_dev);
	getline(&buff, &buff_len, proc_net_dev);
	
        while ( (getline(&buff, &buff_len, proc_net_dev) ) != EOF ) {
                sscanf(buf,"%[^:]",ifname);
		if (strstr(ifname,ifn) == NULL)
			continue;
		
		sscanf(strchr(buf,':')+1, "%*lu %lu %lu %lu %lu %lu %*lu %*lu %*lu %lu %lu %lu %lu %lu", 
						&ret.rx_packets,
						&ret.rx_errs,
						&ret.rx_drop,
						&ret.rx_fifo,
						&ret.rx_frame,
						&ret.tx_packets,
						&ret.tx_errs,
						&ret.tx_drop,
						&ret.tx_fifo,
						&ret.tx_colls);
        }
	
	return (ret);
}
