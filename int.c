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
#include <macro.h>

static char *buf;
static int buf_len;
static int cnt[8];

int
get_interrupt_counter(int i, int *cpu)
{
	FILE *f;
	int interrupt;
	int j;
	char *r;

	j = 0;
	*cpu = 0;
	
	f = fopen(PROC_INTERRUPT,"r");
	if (f == NULL)
		fatal("fopen()");

	if ( getline(&buf,&buf_len,f) == -1 )
		fatal("getline()");

	while ( getline(&buf,&buf_len,f) != EOF ) {
		sscanf(buf,"%d",&interrupt);

		if (i != interrupt)
			continue;

		r = strchr(buf,':');
		if (r == NULL)
			fatal(__INTERNAL__);

		sscanf(	r+1, "%d %d %d %d %d %d %d %d",
			&cnt[0], &cnt[1], &cnt[2], &cnt[3],	
			&cnt[4], &cnt[5], &cnt[6], &cnt[7]);

		while ( cnt[j] == 0 )
			j++;

		*cpu = j;
	}
out:
	close (f);
	return (cnt[j]);
}
