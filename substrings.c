#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include "sufbase.h"
#include "lines.h"

unsigned lcp[N_SUF]; /* length of the longest common prefix with the previous suffix */

int main(void)
{
    add_lines(stdin);
#if 0
    for (unsigned i = 0; i < nsuf; i++)
	printf("%u %s\n", suf[i].count, strtab + suf[i].strix);
#endif
    system("ps up $PPID");
    return 0;
}

/* ex: set ts=8 sts=4 sw=4 noet: */
