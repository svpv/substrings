#include <stdio.h>
#include <assert.h>
#include "sufbase.h"
#include "lines.h"
#include "lcp.h"
#include "ldi.h"

struct stack_ent {
    int i; /* left edge */
    int k; /* representative element with SIL */
};

static struct stack_ent stack[MAXLEN];
static int nstack = 0;

#define Top_i stack[nstack-1].i
#define Top_k stack[nstack-1].k

static void trivial(int i, ldi_cb_t cb)
{
    const char *str = strtab + suf[i].strix;
    if (str[lcp[i+1]] != '\0')
	cb(str, 0, suf[i].count);
}

static void range(int i, int j, int k, ldi_cb_t cb)
{
    assert(j > i);
    int LBL = (lcp[i] > lcp[j+1]) ? lcp[i] : lcp[j+1];
    int SIL = lcp[k];
    if (LBL >= SIL)
	return;
    unsigned cnt = 0;
    for (; i <= j; i++)
	cnt += suf[i].count;
    const char *str = strtab + suf[k].strix;
    cb(str, SIL, cnt);
}

void ldi_walk(ldi_cb_t cb)
{
    stack[nstack++] = (struct stack_ent) { 0, 0 };
    for (unsigned j = 0; j < nsuf; j++) {
	trivial(j, cb);
        while (lcp[j+1] < lcp[Top_k]) {
	    range(Top_i, j, Top_k, cb);
	    nstack--;
	}
	if (lcp[Top_k] == lcp[j+1])
	    Top_k = j+1;
	else {
	    stack[nstack] = (struct stack_ent) { Top_k, j+1 };
	    nstack++;
	}
    }
}
