#include "cons.h"

#include <stdlib.h>
#include <stdio.h>

cons *nucons(void *car, void *cdr) {
  cons *out = malloc(sizeof(cons));
  out->car = car;
  out->cdr = cdr;

  return out;
}

cons *listlast(cons *list) {
  if (!list)
    return NULL;
  
  while (list->cdr)
    list = list->cdr;

  return list;
}

cons *nth(cons *list, unsigned int i) {
  while (list && i--)
    list = list->cdr;

  return list;
}

cons *listpush(cons **list, void *new) {
  cons *ncons = nucons(new, NULL);
  if (!*list)
    *list = ncons;
  else
    listlast(*list)->cdr = ncons;

  return ncons;
}

uint32 listlen(cons *list) {
  uint32 i = 0;
  while (list) {
    i++;
    list = list->cdr;
  }

  return i;
}

void *delnthpop(cons **list, unsigned int i) {
  if (!list || !(*list) || i >= listlen(*list))
    return NULL;

  void *dangling;
  void *next;
    
  if (!i) {
    dangling = (*list)->car;
    cons *ncell = (*list)->cdr;
    free(*list);
    *list = ncell;
    return dangling;
  } else {
    cons *cell = nth(*list, i - 1);
    dangling = (cell->cdr)->car;
    next = (cell->cdr)->cdr;
    free(cell->cdr);
    cell->cdr = next;
    return dangling;
  }
}

void *delcons(cons *prev, cons **ccons) {
  cons *ncons = (*ccons)->cdr;
  if (prev) {
    prev->cdr = ncons;
  }

  void *ret = (*ccons)->car;
  free(*ccons);
  *ccons = ncons;
  return ret;
}

void princ(const cons *list) {
  if (!list) {
    fputs("NULL", stderr);
    return;
  }

  int tab = 0;
  while (1) {
    for (int i = 0; i < tab; i++)
      fprintf(stderr, "  ");

    if (list->cdr) {
      fprintf(stderr, list->car ? "(%p\n" : "(NULL\n", list->car);
      list = list->cdr;
      tab++;
    } else {
      fprintf(stderr, list->car ? "(%p)\n"  : "(NULL)\n", list->car);
      break;
    }
  }
}

void prin1(const cons *list) {
  if (!list) {
    fputs("NULL", stderr);
    return;
  }

  int tab = 0;
  while (1) {
    for (int i = 0; i < tab; i++)
      fprintf(stderr, "  ");

    if (list->cdr) {
      fprintf(stderr, "(%p\n", list->car);
      list = list->cdr;
      tab++;
    } else {
      fprintf(stderr, "(%p)\n", list->car);
      break;
    }
  }
}

// This needed to be quoted somewhere or I was going to explode.
// Painless MySQL HA, Scalability and Flexibility with Ansible, MHA and ProxySQL
// Bash, Docker, Ansible, ProxySQL, MHA, and MySQL! Painless! Next talk: carving a pocket watch out of granite with your dick! Painless!
// -- n-gate.com, 2016
