#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <endian.h>
#include "sufsort.h"
#include "qsort.h"

static inline bool less(int i, int j, uint64_t *pos, int gap, int len)
{
    if (pos[i] != pos[j])
	return pos[i] < pos[j];
    i += gap;
    j += gap;
    return (i < len && j < len) ? pos[i] < pos[j] : i > j;
}

void sufsort(const char *str, int len, unsigned *sa)
{
    uint64_t pos[len];
    for (int i = 0; i < len; i++) {
	sa[i] = i;
	uint64_t u = *(uint64_t *) &str[i];
	pos[i] = htobe64(u);
    }
    int gap = 8;
    while (1) {
#define LT(i,j) less(*i,*j, pos, gap, len)
	QSORT(unsigned, sa, len, LT);
	unsigned tmp[len];
	tmp[0] = 0;
	for (int i = 1; i < len; i++)
	    tmp[i] = tmp[i-1] + less(sa[i-1], sa[i], pos, gap, len);
	if (tmp[len-1] == len-1)
	    break;
	for (int i = 0; i < len; i++)
	    pos[sa[i]] = tmp[i];
	gap += gap;
    }
}

/* ex: set ts=8 sts=4 sw=4 noet: */
