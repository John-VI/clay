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

void *delnthpop(cons **list, unsigned int i) {
  if (!list || !(*list))
    return NULL;

  void *dangling;
  void *next;
    
  if (!i || !(*list)->cdr) {
    dangling = (*list)->car;
    free(*list);
    *list = NULL;
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

void princ(const cons *list) {
  if (!list) {
    puts("NULL");
    return;
  }

  int tab = 0;
  while (1) {
    for (int i = 0; i < tab; i++)
      printf("  ");

    if (list->cdr) {
      printf("(0x%.5X\n" ? list->car : "(NULL\n", list->car);
      list = list->cdr;
      tab++;
    } else {
      printf("(0x%.5X)\n" ? list->car : "(NULL)\n", list->car);
      break;
    }
  }
}

void prin1(const cons *list) {
  if (!list) {
    puts("NULL");
    return;
  }

  int tab = 0;
  while (1) {
    for (int i = 0; i < tab; i++)
      printf("  ");

    if (list->cdr) {
      printf("(0x%X\n", list->car);
      list = list->cdr;
      tab++;
    } else {
      printf("(0x%X)\n", list->car);
      break;
    }
  }
}
