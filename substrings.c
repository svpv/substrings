#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

static char strtab[1<<30];
static unsigned strtab_size; /* place new string at stratab + strtab_size */

struct suffix {
    unsigned strix; /* index into strtab */
    unsigned count; /* that many times added */
};

#define N_SUF ((1U<<31)-(1<<29))
union buf {
    struct suffix suf[N_SUF];
};

static union buf buf1;
static struct suffix *suf = buf1.suf;
static unsigned nsuf;

static union buf buf2;
static struct suffix *tmpsuf = buf2.suf;

#include "sufsort.h"

static void add_sorted_suffixes(unsigned strix, unsigned len, unsigned cnt)
{
    unsigned sa[len];
    sufsort(strtab + strix, len, sa);
    for (int i = 0; i < len; i++)
	suf[nsuf+i] = (struct suffix) { strix + sa[i], cnt };
#if 0
    for (int i = nsuf + 1; i < nsuf + len; i++) {
	const char *str1 = strtab+suf[i-1].strix;
	const char *str2 = strtab+suf[i  ].strix;
	assert(*str1);
	assert(*str2);
	assert(str1 != str2);
	int cmp = strcmp(str1, str2);
	if (cmp >= 0)
	    fprintf(stderr, "\tbad cmp:\n%s\n%s\n", str1, str2);
    }
#endif
    nsuf += len;
}

struct stack_ent {
    unsigned suf0;
    unsigned size;
    unsigned linecnt;
};

struct stack_ent stack[32];
unsigned nstack;

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

static void push_for_merge(unsigned suf0, unsigned size)
{
    stack[nstack++] = (struct stack_ent) { suf0, size, 1 };
    while (nstack > 1 && stack[nstack-1].linecnt == stack[nstack-2].linecnt)
	merge1();
}

static void add_line(const char *line, unsigned len)
{
    const char *line0 = line;
    while (*line == ' ')
	line++;
    int c = *line;
    assert(c >= '0' && c <= '9');
    unsigned cnt = 0;
    do {
	unsigned cnt0 = cnt;
	cnt = cnt0 * 10 + c - '0';
	assert(cnt >= cnt0);
	c = *++line;
    }
    while (c >= '0' && c <= '9');
    assert(cnt);
    assert(c == ' ');
    c = *++line;
    assert(c);
    len -= (line - line0);
    assert(len);
    assert(line[len] == '\0');
    memcpy(strtab + strtab_size, line, len + 1);
    assert(line[len] == '\0');
    unsigned strix = strtab_size;
    strtab_size += len + 1;
    unsigned suf0 = nsuf;
    add_sorted_suffixes(strix, len, cnt);
    push_for_merge(suf0, len);
}

static void add_lines(void)
{
    char *line = NULL;
    size_t alloc_size = 0;
    ssize_t len;
    while ((len = getline(&line, &alloc_size, stdin)) >= 0) {
	if (len > 0 && line[len-1] == '\n')
	    line[--len] = '\0';
	if (len == 0)
	    continue;
	add_line(line, len);
    }
    free(line);
    while (nstack > 1)
	merge1();
}

unsigned lcp[N_SUF]; /* length of the longest common prefix with the previous suffix */

int main(void)
{
    add_lines();
#if 0
    for (unsigned i = 0; i < nsuf; i++)
	printf("%u %s\n", suf[i].count, strtab + suf[i].strix);
#endif
    system("ps up $PPID");
    return 0;
}

/* ex: set ts=8 sts=4 sw=4 noet: */
