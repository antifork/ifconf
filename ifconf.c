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
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <sysexits.h>
#define _GNU_SOURCE
#include <getopt.h>

#define __KERNEL__
#include <asm/types.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>

#include <config.h>
#include <typedef.h>
#include <macro.h>

static 
const char usage_s[] = "\
Usage:%s [options]                                               \n\
   -a, --all            display all interfaces                   \n\
   -v, --version        display the version and exit.            \n\
   -h, --help           print this help.                         \n";

static
const char license[]="\n\
Copyright (c) 2004 Nicola Bonelli <bonelli@antifork.org>.\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.";

static const struct option long_options[] = {
	{"version", no_argument, NULL, 'v'},
	{"all",     no_argument, NULL, 'S'},
	{"help",    no_argument, NULL, 'h'},
	{ NULL,     0          , NULL,  0 }};

struct {
	int	all;
} options;


/* prototype */
struct ethtool_drvinfo *ethernet_info(const char *);
char * get_ifname();
stats_t get_stats(char *);


/* verr()/verrx() wrapper */
__dead void
fatal(const char *pattern,...)
{
        va_list ap;

        va_start(ap, pattern);
        if (errno)
                verr(EX_SOFTWARE, pattern, ap);
        else
                verrx(EX_SOFTWARE, pattern, ap);
}


/* usage */
__dead void
usage()
{
        extern char *__progname;
        printf(usage_s, __progname);
        exit(0);
}


static char if_buffer[10240];


const char *if_flag[]= {
"UP", "BROADCAST", "DEBUG", "LOOPBACK", "PTP", "NOTRL", "RUNNING", "NOARP", 
"PROMIS", "ALLMULTI", "MASTER", "SLAVE", "MULTICAST", "PORTSEL", "AUTOMEDIA" };


void
if_browser()
{
	struct ifreq ifreq_io;
	struct ifconf ifc;
	struct ethtool_drvinfo *info;
	short int ifru_flags;
	unsigned int base_addr;
	int sd, int_cnt, int_cpu;
	char *ifrname;
	stats_t s;

	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
		fatal("socket()");

	while (ifrname = get_ifname())
	{
                /* reset Graphics Rendition */
                printf("\e[0m");

		strncpy(ifreq_io.ifr_name, ifrname,IFNAMSIZ);
		if (ioctl(sd, SIOCGIFFLAGS, &ifreq_io) < 0) {
			continue;
		}

		/* save flags */
		ifru_flags= ifreq_io.ifr_flags;

		/* filters: IFF_UP */
		if ( !options.all && (ifreq_io.ifr_flags & IFF_UP) == 0)
			continue;

		/* print interface name */
		printf("%s\t",ifrname);

		/* mii test link */
		mii_testlink(ifrname);

		/* SIOCGIFHWADDR */
       		strncpy(ifreq_io.ifr_name, ifrname,IFNAMSIZ); 
	        if (ioctl(sd, SIOCGIFHWADDR, &ifreq_io) != -1 ) {
			struct ether_addr *eth_addr;
                	eth_addr = (struct ether_addr *) & ifreq_io.ifr_addr.sa_data;
                	printf("HWaddr %s ", ether_ntoa(eth_addr));
                }

                /* FLAG */
                {
                        int i,j=0;
                        for (i=1;i<16;i++) 
                                if ( ifru_flags & BIT(i) ) 
                                        printf(j++ == 0 ? "\n\t%s " : "%s ", if_flag[i-1]);
				
                        
                }

                /* SIOCGIFHWADDR */
                strncpy(ifreq_io.ifr_name, ifrname,IFNAMSIZ);
                if (ioctl(sd, SIOCGIFMTU, &ifreq_io) != -1 ) {
                        printf("MTU:%d ", ifreq_io.ifr_mtu);
                }

                /* SIOCGIFMETRIC */
                strncpy(ifreq_io.ifr_name, ifrname,IFNAMSIZ);
                if (ioctl(sd, SIOCGIFMETRIC, &ifreq_io) != -1 ) {
                        printf("metric:%d ", ifreq_io.ifr_metric ? ifreq_io.ifr_metric : 1);
                }

		printf("\n");

                /* SIOCGIFADDR */
                strncpy(ifreq_io.ifr_name, ifrname,IFNAMSIZ);
                if (ioctl(sd, SIOCGIFADDR, &ifreq_io) != -1 ) {
			struct sockaddr_in *p;
			p = (struct sockaddr_in *)&ifreq_io.ifr_addr;
			
			if(ifreq_io.ifr_addr.sa_family == AF_INET) {
				printf("\tinet addr:%s ", inet_ntoa(p->sin_addr));	
			}
			else
				goto af_inet_end;
                }
		else
			goto af_inet_end;

	
		/* SIOCGIFBRDADDR */
		strncpy(ifreq_io.ifr_name, ifrname,IFNAMSIZ);
                if (ioctl(sd, SIOCGIFBRDADDR, &ifreq_io) != -1) {
			struct sockaddr_in *p;
			p = (struct sockaddr_in *)&ifreq_io.ifr_addr;
			printf("bcast:%s ", inet_ntoa(p->sin_addr));
			fflush(stdout);
                } 

		/* SIOCGIFNETMASK */
		strncpy(ifreq_io.ifr_name, ifrname,IFNAMSIZ);
		if (ioctl(sd, SIOCGIFNETMASK, &ifreq_io) != -1) {
			struct sockaddr_in *p;
			p = (struct sockaddr_in *)&ifreq_io.ifr_addr;
			printf("mask:%s", inet_ntoa(p->sin_addr));
			fflush(stdout);
		}

		printf("\n");
af_inet_end:
	
		inet6_print_addr(ifrname);
	
		/* SIOCGIFMAP */
		strncpy(ifreq_io.ifr_name, ifrname,IFNAMSIZ);
                if (ioctl(sd, SIOCGIFMAP, &ifreq_io) != -1) {
			base_addr = ifreq_io.ifr_ifru.ifru_map.base_addr;
                }
		else
			base_addr = 0xffff;

		int_cnt = get_interrupt_counter((unsigned int)ifreq_io.ifr_ifru.ifru_map.irq, &int_cpu);

		printf( ifreq_io.ifr_ifru.ifru_map.mem_start ? 
			"\tbase_addr:%1$x memory:%2$x-%3$x irq:%4$d #cpu0=%5$d dma:%6$d port:%7$d\n " : 
			"\tbase_addr:%1$x irq:%4$d #cpu%8$d=%5$d dma:%6$d port:%7$d\n " ,
			(unsigned int)ifreq_io.ifr_ifru.ifru_map.base_addr,
			(unsigned int)ifreq_io.ifr_ifru.ifru_map.mem_start,
			(unsigned int)ifreq_io.ifr_ifru.ifru_map.mem_end,
			(unsigned int)ifreq_io.ifr_ifru.ifru_map.irq,
			(unsigned int)int_cnt,
			(unsigned int)ifreq_io.ifr_ifru.ifru_map.dma,
			(unsigned int)ifreq_io.ifr_ifru.ifru_map.port,
			(unsigned int)int_cpu);	

		/* stats */

		s = get_stats(ifreq_io.ifr_name);
		printf("\tRX packets:%lu errors:%lu dropped:%lu overruns:%lu frame:%lu\n"
		       "\tTx packets:%lu errors:%lu dropped:%lu overruns:%lu carrier:%lu\n",
			s.rx_packets,
			s.rx_errs,
			s.rx_drop,
			s.rx_fifo,
			s.rx_frame,
			s.tx_packets,
 			s.tx_errs,
                        s.tx_drop,
                        s.tx_fifo,
                        s.tx_colls);
		
		/* print ethernet_info */
		info=ethernet_info(ifrname);
		if (info != NULL)
			printf("\tether_driver:%s version:%s\n", info->driver, info->version);

		putchar('\n');
	}


	close(sd);
	return;
}


__dead void the_function_after() __attribute__((destructor));
__dead void
the_function_after()
{
  	/* reset Graphics Rendition */
        printf("\e[0m");
	exit(0);
}


int
main(argc,argv)
	int argc;
	char **argv;
{
	int i;

	while ((i = getopt_long(argc, argv, "hva", long_options, 0)) != EOF)
                switch (i) {
                        case 'h':
                                usage();
                        case 'v':
			        fprintf(stderr, "%s\n\nifconf %s\n", license, VERSION);
                        	exit(0);
                        case 'a':
				options.all=1 ;
				break;
                        case '?': fatal(__INTERNAL__);
        }

	argc -= optind;
	argv += optind;

	signal(SIGINT,exit);	
	if_browser();

	return (0);
}
