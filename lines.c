#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sufbase.h"
#include "sufsort.h"
#include "sufmerge.h"
#include "lines.h"

static void add_suffixes(unsigned strix, unsigned len, unsigned cnt)
{
    unsigned sa[len];
    sufsort(strtab + strix, len, sa);
    for (int i = 0; i < len; i++)
	suf[nsuf+i] = (struct suffix) { strix + sa[i], cnt };
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
    unsigned suf0 = nsuf;
    add_suffixes(strix, len, cnt);
    push_for_merge(suf0, len);
}

void add_lines(FILE *fp)
{
    char *line = NULL;
    size_t alloc_size = 0;
    ssize_t len;
    while ((len = getline(&line, &alloc_size, fp)) >= 0) {
	if (len > 0 && line[len-1] == '\n')
	    line[--len] = '\0';
	if (len == 0)
	    continue;
	add_line(line, len);
    }
    free(line);
    finish_merge();
}
