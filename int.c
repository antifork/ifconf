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

#include <stdio.h>

#define PROC_INTERRUPT	"/proc/interrupts"

static char *buf;
static int buf_len;
static int _cnt[8];

int
get_interrupt_counter(int i, int *cpu)
{
	FILE *f;
	int _int;
	int j;

	j = 0;
	*cpu = 0;
	
	f = fopen(PROC_INTERRUPT,"r");
	if (f == NULL)
		fatal("fopen()");

	if ( getline(&buf,&buf_len,f) == -1 )
		fatal("getline()");

	while ( getline(&buf,&buf_len,f) != EOF ) {
		sscanf(buf,"%d",&_int);
		if (i != _int)
			continue;

		sscanf(strchr(buf,':')+1,"%d %d %d %d %d %d %d %d",&_cnt[0], &_cnt[1], &_cnt[2], &_cnt[3],	
								   &_cnt[4], &_cnt[5], &_cnt[6], &_cnt[7]);
		while ( _cnt[j] == 0 )
			j++;

		*cpu = j;
	}
out:
	close (f);
	return (_cnt[j]);
}
