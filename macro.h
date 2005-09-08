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

#ifndef __unused
#define __unused        __attribute__((unused))
#endif
#ifndef __dead
#define __dead          __attribute__((noreturn))
#endif

#define __INTERNAL__    "%d:%s() internal error",\
                        __LINE__,__FUNCTION__

// Set Graphics Rendition
#define SGR_reset	"\e[0m"
#define SGR_bold	"\e[0;1m"

#define BIT(i)  	(1<<(i-1))
#define PROC_NET_DEV 	"/proc/net/dev"
#define PROC_INTERRUPT  "/proc/interrupts"
#define PROC_IFINET6    "/proc/net/if_inet6"

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

