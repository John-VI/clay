#ifndef CLAY_CONS
#define CLAY_CONS

#include "alias_t.h"

typedef struct cons {
    void *car;
    struct cons *cdr;
} cons;

void princ(const cons *);
void prin1(const cons *);

cons *nth(cons *, unsigned int);
cons *listlast(cons *);

cons *nucons(void *, void *);
cons *listpush(cons **, void *);
void *delnthpop(cons **, unsigned int);

uint32 listlen(cons *);

#endif
