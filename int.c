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
