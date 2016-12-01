#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include "sufbase.h"
#include "lines.h"
#include "lcp.h"
#include "ldi.h"

/* We use a very simple model to estimate profit: count * length.
 * It's rough, but it's not wrong.  The model is good at throwing
 * away junk data.  You can refine the model later, provided that
 * you have enough backlog. */
static int min_profit = 2048;
static int min_count = 1024;

static void substring(const char *str, unsigned len, unsigned cnt)
{
    if (cnt < (unsigned) min_count)
	return;
    if (len == 0)
	len = strlen(str);
    unsigned profit = cnt * len;
    if (profit < (unsigned) min_profit)
	return;
    printf("%u\t%.*s\n", cnt, len, str);
}

int hascnt;
int prefixes;

int main(int argc, char **argv)
{
    enum {
	OPT_MIN_COUNT  = 256,
	OPT_MIN_PROFIT = 257,
    };
    static const struct option longopts[] = {
	{ "min-count",        1, 0, OPT_MIN_COUNT },
	{ "min-profit",       1, 0, OPT_MIN_PROFIT },
	{ "lines-have-count", 0, &hascnt, 1 },
	{ "prefixes-only",    0, &prefixes, 1 },
	{  NULL,              0, 0, 0 },
    };
    int c;
    while ((c = getopt_long(argc, argv, "", longopts, NULL)) != -1) {
	switch (c) {
	case 0:
	    break;
	case OPT_MIN_COUNT:
	    min_count = atoi(optarg);
	    assert(min_count > 0);
	    break;
	case OPT_MIN_PROFIT:
	    min_profit = atoi(optarg);
	    assert(min_profit > 0);
	    break;
	default:
	    fprintf(stderr, "Usage: substrings\n");
	    return 1;
	}
    }
    add_lines(stdin, hascnt, prefixes);
    make_lcp();
    ldi_walk(substring);
    return 0;
}
