#include "sufbase.h"

char strtab[N_STRTAB];
unsigned strtab_size;

union buf buf1;
struct suffix *suf = buf1.suf;
unsigned nsuf;

union buf buf2;
struct suffix *tmpsuf = buf2.suf;
