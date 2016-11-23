#include <string.h>
#include <stdlib.h>

static int sufcmp(const void *pstr1, const void *pstr2)
{
    return strcmp(*(const char **) pstr1, *(const char **) pstr2);
}

void sufsort(const char *str, int len, unsigned *sa)
{
    const char *suf[len];
    for (int i = 0; i < len; i++)
	suf[i] = str + i;
    qsort(suf, len, sizeof *suf, sufcmp);
    for (int i = 0; i < len; i++)
	sa[i] = suf[i] - str;
}

/* ex: set ts=8 sts=4 sw=4 noet: */
