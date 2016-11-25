#include <stdio.h>
#include <string.h>
#include "sufbase.h"
#include "lines.h"
#include "lcp.h"
#include "ldi.h"

/* We use a very simple model to estimate profit: count * length.
 * It's rough, but it's not wrong.  The model is good at throwing
 * away junk data.  You can refine the model later, provided that
 * you have enough backlog. */
static unsigned min_profit = 2048;

static void substring(const char *str, unsigned len, unsigned cnt)
{
    if (len == 0)
	len = strlen(str);
    unsigned profit = cnt * len;
    if (profit < min_profit)
	return;
    printf("%u\t%.*s\n", cnt, len, str);
}

int main(void)
{
    add_lines(stdin);
    make_lcp();
    ldi_walk(substring);
    return 0;
}
