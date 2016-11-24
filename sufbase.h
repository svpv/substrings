#define N_STRTAB (1<<30)
extern char strtab[];
extern unsigned strtab_size; /* place new string at stratab + strtab_size */

struct suffix {
    unsigned strix; /* index into strtab */
    unsigned count; /* that many times added */
};

#define N_SUF ((1U<<31)-(1<<29))
union buf {
    struct suffix suf[N_SUF];
    unsigned lcp[N_SUF];
};

extern union buf buf1;
extern struct suffix *suf;
extern unsigned nsuf;

extern union buf buf2;
extern struct suffix *tmpsuf;
