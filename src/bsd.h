#ifndef CLAY_BSD
#define CLAY_BSD

#include <sys/types.h>

size_t strlcpy(char *, const char *, size_t);
size_t strlcat(char *, const char *, size_t);
char *strsep(char **, const char *);

ssize_t getdelim(char ** restrict lineptr, size_t * restrict n, int delimiter, FILE * restrict stream);
ssize_t getline(char ** restrict lineptr, size_t * restrict n, FILE * restrict stream);

char *chomp(const char *); // Yeah this totally exists on BSD. Don't you trust me?

char *elfypath(const char *);

#endif
