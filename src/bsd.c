/*	$OpenBSD: strlcat.c,v 1.19 2019/01/25 00:19:25 millert Exp $	*/

/*
 * Copyright (c) 1998, 2015 Todd C. Miller <millert@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "bsd.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <SDL3/SDL_filesystem.h>

/*
 * Appends src to string dst of size dsize (unlike strncat, dsize is the
 * full size of dst, not space left).  At most dsize-1 characters
 * will be copied.  Always NUL terminates (unless dsize <= strlen(dst)).
 * Returns strlen(src) + MIN(dsize, strlen(initial dst)).
 * If retval >= dsize, truncation occurred.
 */
size_t
strlcat(char *dst, const char *src, size_t dsize)
{
	const char *odst = dst;
	const char *osrc = src;
	size_t n = dsize;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end. */
	while (n-- != 0 && *dst != '\0')
		dst++;
	dlen = dst - odst;
	n = dsize - dlen;

	if (n-- == 0)
		return(dlen + strlen(src));
	while (*src != '\0') {
		if (n != 0) {
			*dst++ = *src;
			n--;
		}
		src++;
	}
	*dst = '\0';

	return(dlen + (src - osrc));	/* count does not include NUL */
}

/*
 * Copy string src to buffer dst of size dsize.  At most dsize-1
 * chars will be copied.  Always NUL terminates (unless dsize == 0).
 * Returns strlen(src); if retval >= dsize, truncation occurred.
 */
size_t
strlcpy(char *dst, const char *src, size_t dsize)
{
	const char *osrc = src;
	size_t nleft = dsize;

	/* Copy as many bytes as will fit. */
	if (nleft != 0) {
		while (--nleft != 0) {
			if ((*dst++ = *src++) == '\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src. */
	if (nleft == 0) {
		if (dsize != 0)
			*dst = '\0';		/* NUL-terminate dst */
		while (*src++)
			;
	}

	return(src - osrc - 1);	/* count does not include NUL */
}

char *
strsep(char **stringp, const char *delim)
{
	char *s;
	const char *spanp;
	int c, sc;
	char *tok;

	if ((s = *stringp) == NULL)
		return (NULL);
	for (tok = s;;) {
		c = *s++;
		spanp = delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*stringp = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

/* Anyway */

ssize_t
getdelim(char ** restrict lineptr, size_t * restrict n, int delimiter, FILE * restrict stream) {
  // This is POSIXly completely wrong btw.
  size_t size;
  char c;
  ssize_t i;

  if ((!n || *n) && *lineptr) {
    errno = EINVAL;
    return -1; //If you want to give me a buffer but not the size then you can fk right off.
  }
  
  if (n && *n)
    size = n;
  else
    size = 55; // The average line length for text file in my documents folder is 49.

  if (!*lineptr)
    *lineptr = malloc(sizeof(char) * size);
  // If lineptr isn't real then we make it ourselves according to n, assuming that exists.
  // if lineptr is real then n is assumed to be its size. Important: Don't fk this up.

  for (i = 0; (c = getc(stream)) != EOF && ((*lineptr)[i] = c) != delimiter; i++) {
    if (i >= size - 1) { // We still need room for the \0.
      size *= 2;
      realloc(*lineptr, size);
    }
  }

  (*lineptr)[i] = '\0';

  if (n)
    *n = size;
  // Tell them our new buffer size if they care to know.

  return i;
}

ssize_t
getline(char ** restrict lineptr, size_t * restrict n, FILE * restrict stream) {
  return getdelim(lineptr, n, '\n', stream); // This c sh easy.
}

char
*chomp(const char *str) {
  if (!str) {
    errno = EINVAL;
    return NULL; // F off
  }

  size_t len = strlen(str);

  if (len == 0)
    return calloc(1, sizeof(char));

  char *strptr = str;
  char *endptr = str + len;

  while (isspace(*strptr))
    strptr++;

  while (endptr > strptr && isspace(*endptr))
    endptr--;

  if (endptr <= strptr)
    return calloc(1, sizeof(char));
  
  char *out = malloc(endptr - strptr + 1);
  memcpy(out, strptr, endptr - strptr);
  out[endptr-strptr] = '\0';

  return out;
}

char
*elfypath(const char *path) {
  const size_t pathlen = strlen(SDL_GetBasePath()) + strlen(path) + 1;
  char *fullpath = malloc(pathlen);
  strlcpy(fullpath, SDL_GetBasePath(), pathlen);
  strlcat(fullpath, path, pathlen);
  return fullpath;
}

bool
blankp(const char *str) {
	while (*(str))
		if (!isspace(*str++))
			return false;
	return true;
}
