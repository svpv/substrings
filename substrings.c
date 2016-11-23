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

#define N_SUF (1U<<31)
static struct suffix suf[N_SUF];
static unsigned nsuf;

static int sufcmp(const void *s1, const void *s2)
{
    const struct suffix *suf1 = s1;
    const struct suffix *suf2 = s2;
    return strcmp(strtab + suf1->strix, strtab + suf2->strix);
}

static void sort_suffixes(void)
{
    fprintf(stderr, "qsort... ");
    qsort(suf, nsuf, sizeof suf[0], sufcmp);
    int i, j;
    for (i = 0, j = 0; i < nsuf; i++) {
	while (i + 1 < nsuf && sufcmp(suf + i, suf + i + 1) == 0)
	    i++;
	suf[j++] = suf[i];
    }
    fprintf(stderr, "reduced from %9u to %9u\n", nsuf, j);
    nsuf = j;
}

static unsigned sort_thr = 1 << 27; /* threshold for interim sorting,
				       128M suffixes * 8bytes = 1G */

static void add_suffix(const char *line, unsigned cnt)
{
    unsigned strix = line - strtab;
    assert(nsuf < N_SUF);
    suf[nsuf].strix = strix;
    suf[nsuf].count = cnt;
    bool sort = nsuf == sort_thr;
    nsuf++;
#if 0
    if (sort) {
	sort_suffixes();
	sort_thr += sort_thr / 2;
    }
#endif
}

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
    add_sorted_suffixes(strix, len, cnt);
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
}

unsigned lcp[N_SUF]; /* length of the longest common prefix with the previous suffix */

int main(void)
{
    add_lines();
#if 0
    sort_suffixes();
    for (unsigned i = 0; i < nsuf; i++)
	printf("%u %s\n", suf[i].count, strtab + suf[i].strix);
#endif
    system("ps up $PPID");
    return 0;
}

/* ex: set ts=8 sts=4 sw=4 noet: */
