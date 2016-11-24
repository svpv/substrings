#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "sufbase.h"
#include "sufmerge.h"

struct stack_ent {
    unsigned suf0;
    unsigned size;
    unsigned linecnt;
};

static struct stack_ent stack[32];
static unsigned nstack;

static void merge1(void)
{
    unsigned suf1 = stack[nstack-1].suf0;
    unsigned suf2 = stack[nstack-2].suf0;
    unsigned suf1end = suf1 + stack[nstack-1].size;
    unsigned suf2end = suf2 + stack[nstack-2].size;
    /* Merge suf1 and suf2 into out. */
    struct suffix *out0 = tmpsuf, *out = out0;
    while (1) {
	int cmp = strcmp(strtab + suf[suf1].strix, strtab + suf[suf2].strix);
	if (cmp < 0) {
	    *out++ = suf[suf1++];
	    if (suf1 == suf1end) break;
	}
	else if (cmp > 0) {
	    *out++ = suf[suf2++];
	    if (suf2 == suf2end) break;
	}
	else {
	    unsigned cnt = suf[suf1].count + suf[suf2].count;
	    *out++ = (struct suffix) { suf[suf2].strix, cnt };
	    suf1++; suf2++; nsuf--;
	    if (suf1 == suf1end) break;
	    if (suf2 == suf2end) break;
	}
    }
    /* Append what's left. */
    while (suf1 < suf1end)
	*out++ = suf[suf1++];
    while (suf2 < suf2end)
	*out++ = suf[suf2++];
    /* Merge is complete, mend the stack. */
    nstack--;
    stack[nstack-1].size = out - out0;
    stack[nstack-1].linecnt += stack[nstack].linecnt;
    /* Either write back or switch (suf,tmpsuf) buffers. */
    if (nstack > 1) {
	struct suffix *back = suf + stack[nstack-1].suf0;
	memcpy(back, out0, (char *) out - (char *) out0);
    }
    else {
	struct suffix *hold = suf;
	suf = tmpsuf;
	tmpsuf = hold;
	if (stack[nstack-1].linecnt >= (1<<20))
	    fprintf(stderr, "merged %u lines into %u suffixes\n",
		    stack[nstack-1].linecnt, stack[nstack-1].size);
	assert(stack[nstack-1].size == nsuf);
    }
}

void push_for_merge(unsigned suf0, unsigned size)
{
    stack[nstack++] = (struct stack_ent) { suf0, size, 1 };
    while (nstack > 1 && stack[nstack-1].linecnt == stack[nstack-2].linecnt)
	merge1();
}

void finish_merge(void)
{
    while (nstack > 1)
	merge1();
}
