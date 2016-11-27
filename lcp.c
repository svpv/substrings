#include <stdint.h>
#include <endian.h>
#include "sufbase.h"
#include "lcp.h"

static int lcp2(const char *s1, const char *s2)
{
    const char *s0 = s1;
    while (1) {
	uint64_t u1 = *(uint64_t *) s1;
	uint64_t u2 = *(uint64_t *) s2;
	/* we do not have dups, no need to check for null byte */
	if (u1 != u2) {
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN
	    if ((unsigned) u1 == (unsigned) u2)
		s1 += 4, s2 += 4;
#endif
	    break;
	}
	s1 += 8, s2 += 8;
    }
    while (*s1 == *s2)
	s1++, s2++;
    return s1 - s0;
}

unsigned *lcp;

void make_lcp(void)
{
    lcp = (suf == buf1.suf) ? buf2.lcp : buf1.lcp;
    lcp[0] = 0;
    for (int i = 1; i < nsuf; i++)
	lcp[i] = lcp2(strtab + suf[i-1].strix, strtab + suf[i].strix);
    lcp[nsuf] = 0;
}
