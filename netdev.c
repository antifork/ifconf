/*
    $Id$

    Copyright (c) 2003 Nicola Bonelli <bonelli@antifork.org>
                                       <bonelli@netserv.iet.unipi.it>


    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

#define __KERNEL__
#include <asm/types.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sysexits.h>

#include <config.h>

/*
 * given the ifname, the function return a pointer to
 * a static struct ethtool_drvinfo defined in Linux ethtool.h
 */
struct ethtool_drvinfo *
ethernet_info(const char *ifname)
{
	static struct ethtool_drvinfo *info;
	struct ifreq ifr;
	int s;
	u32 req = ETHTOOL_GDRVINFO;	/* netdev ethcmd */

	s = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (s == -1) {
		printf("socket(): %s\n", strerror(errno));
		return NULL;
	}

	strncpy(ifr.ifr_name, ifname,IF_NAMESIZE);
	free(info);

	ifr.ifr_data = (__caddr_t) malloc(sizeof(struct ethtool_drvinfo));
	if (ifr.ifr_data == NULL) {
		printf("malloc(): %s\n", strerror(errno));
		return NULL;
	}

	info = (struct ethtool_drvinfo *) ifr.ifr_data;
	strncpy(ifr.ifr_data, (char *) &req, sizeof(req));

	if (ioctl(s, SIOCETHTOOL, &ifr) == -1) {
		//printf("%s: netdrv does not support SIOCETHTOOL ioctl()\n",ifname);
		errno = 0;
		return NULL;
	}
	return (info);
}


/* 
 *   The following functions are based on Donald Becker's "mii-diag" program. 
 *   mii-diag is written/copyright 1997-2000 by Donald Becker <becker@scyld.com>
 */

/* This data structure is used for all the MII ioctl's */
struct mii_data {
    __u16       phy_id;
    __u16       reg_num;
    __u16       val_in;
    __u16       val_out;
};

enum mii_register {
	mii_bmcr=0,	/* Basic Mode Control Register */
#define  MII_BMCR_RESET         0x8000
#define  MII_BMCR_LOOPBACK      0x4000
#define  MII_BMCR_100MBIT       0x2000
#define  MII_BMCR_AN_ENA        0x1000
#define  MII_BMCR_ISOLATE       0x0400
#define  MII_BMCR_RESTART       0x0200
#define  MII_BMCR_DUPLEX        0x0100
#define  MII_BMCR_COLTEST       0x0080
	mii_bmsr,	/* Basic Mode Status Register  */
#define  MII_BMSR_CAP_MASK      0xf800
#define  MII_BMSR_100BASET4     0x8000
#define  MII_BMSR_100BASETX_FD  0x4000
#define  MII_BMSR_100BASETX_HD  0x2000
#define  MII_BMSR_10BASET_FD    0x1000
#define  MII_BMSR_10BASET_HD    0x0800
#define  MII_BMSR_NO_PREAMBLE   0x0040
#define  MII_BMSR_AN_COMPLETE   0x0020
#define  MII_BMSR_REMOTE_FAULT  0x0010
#define  MII_BMSR_AN_ABLE       0x0008
#define  MII_BMSR_LINK_VALID    0x0004
#define  MII_BMSR_JABBER        0x0002
#define  MII_BMSR_EXT_CAP       0x0001
	mii_phy_id1,
	mii_phy_1d2,
	mii_anar,	/* Auto-Negotiation Advertisement Register */
	mii_anlpar,	/* Auto-Negotiation Link Partner Ability Register */
#define  MII_AN_NEXT_PAGE       0x8000
#define  MII_AN_ACK             0x4000
#define  MII_AN_REMOTE_FAULT    0x2000
#define  MII_AN_ABILITY_MASK    0x07e0
#define  MII_AN_FLOW_CONTROL    0x0400
#define  MII_AN_100BASET4       0x0200
#define  MII_AN_100BASETX_FD    0x0100
#define  MII_AN_100BASETX_HD    0x0080
#define  MII_AN_10BASET_FD      0x0040
#define  MII_AN_10BASET_HD      0x0020
#define  MII_AN_PROT_MASK       0x001f
#define  MII_AN_PROT_802_3      0x0001
	mii_aner,	/* Auto-Negotiation Expansion Register */
#define  MII_ANER_MULT_FAULT    0x0010
#define  MII_ANER_LP_NP_ABLE    0x0008
#define  MII_ANER_NP_ABLE       0x0004
#define  MII_ANER_PAGE_RX       0x0002
#define  MII_ANER_LP_AN_ABLE    0x0001
};


static const struct {
    char        *name;
    u_short     value;
} media[] = {
    /* The order through 100baseT4 matches bits in the BMSR */
    { "10baseT-HD",     MII_AN_10BASET_HD },
    { "10baseT-FD",     MII_AN_10BASET_FD },
    { "100baseTx-HD",   MII_AN_100BASETX_HD },
    { "100baseTx-FD",   MII_AN_100BASETX_FD },
    { "100baseT4",      MII_AN_100BASET4 },
    { "100baseTx",      MII_AN_100BASETX_FD | MII_AN_100BASETX_HD },
    { "10baseT",        MII_AN_10BASET_FD | MII_AN_10BASET_HD },
};


/* 
 * test link status using MII tool (when supported)
 */
void
mii_testlink(char *ifname) 
{
	static struct ifreq ifr;
	struct mii_data *mii= (struct mii_data *)&ifr.ifr_data;
	int i,fd, mask, mii_reg[8];
	
	/* open ioctl socket */
	fd = socket(AF_INET, SOCK_DGRAM,0);
	if (fd == -1)
		fatal("socket");

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    	if (ioctl(fd, SIOCGMIIPHY, &ifr) < 0) {
		printf ("(%s) device does not support MII tool ",ifname);
       		goto mii_exit; 
    	}

	/* read 8 registers */
	for (i=0;i<8;i++) {
		mii->reg_num = i;
    		if (ioctl(fd, SIOCGMIIREG, &ifr) < 0) {
			printf ("(%s) device does not support MII tool ",ifname); 
			break;
		}	
		mii_reg[i]=mii->val_out;
	}
	
	/* check for autonegotiation */
	if (!(mii_reg[mii_bmcr]  & MII_BMCR_AN_ENA )) 
		goto an_disabled;	/* autonegotiation disabled */ 

	/* autonegotiation complete? */
	if ( !(mii_reg[mii_bmsr] & MII_BMSR_AN_COMPLETE ))
		goto an_restarted;

	/* autonegotiation ok? */
	if ( (mii_reg[mii_anar] & mii_reg[mii_anlpar]) == 0 ) 	
		goto an_failed;

	//printf ("%s: ",ifname);
	mask = mii_reg[mii_anar] & mii_reg[mii_anlpar];
	mask >>= 5;

	/* set bold */
	if ( (mii_reg[mii_bmsr] & MII_BMSR_LINK_VALID ) )
		printf("\e[0;1m");

        printf("link:");

        for (i = 4; i >= 0; i--) {
        	if (mask & (1<<i)) {
        		printf("%s",media[i].name);
        		break;
        	}
        }
	goto test_link;		

an_failed:
	printf ("(%s) autonegotiation failed ", ifname);
	goto test_link;

an_restarted:
	printf ("(%s) autonegotiation restarted ",ifname);
	goto test_link;

an_disabled:
	printf ("(%s) %s Mbit, %s duplex",
			ifname,	
			( mii_reg[mii_bmcr] & MII_BMCR_100MBIT) ? "100" : "10" ,
			( mii_reg[mii_bmcr] & MII_BMCR_DUPLEX ) ? "full" : "half");			

test_link:
	if ( !(mii_reg[mii_bmsr] & MII_BMSR_LINK_VALID ) )
		printf("(no link) ");
	else
		printf("(ok) ");
mii_exit:
	close (fd);
}

