//#include "aniloader.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "anidata.h"
#include "cons.h"
#include "alias_t.h"
#include "bsd.h"

enum aniptype {
  SHTNAME,
  SHTTEXT,

  DATNAME,
  DATFRMS,
  DATCOLS,
  DATRECT,
  DATFLAG,
  DATDLAY,

  BAD,
  UNPARSE,
  ARG
};

enum argtype {
  NIL,
  INT,
  UINT,
  STR,
};

struct anitoken {
  enum aniptype type;
  uint32 pos;
  char *data;
};

struct aniextok {
  const char *str;
  const uint32 args;
  const enum argtype argtype;
};

const struct aniextok anitlookup[ARG+1] = {
  { "sheetname",    1, STR  },
  { "sheettexture", 1, STR  },

  { "name",         1, STR  },
  { "frames",       1, UINT },
  { "columns",      1, UINT },
  { "rectangle",    4, INT  },
  { "flags",        1, NIL  },
  { "delay",        1, UINT },

  { "",             0, NIL  },
  { "",             0, NIL  },
  { "",             0, NIL  }
};

cons *ingestdata(FILE *f) {
  cons *tree = NULL;
  char *line = NULL;
  size_t buflen = 0;
  int len = 0;

  while ((len = getline(&line, &buflen, f)) != -1) {
    if (blankp(line))
      continue;

    char *baseptr = line;
    char *argptr = line;
    cons *ccell = listpush(&tree, malloc(sizeof(struct anitoken)));
    uint32 rank = 0;

    do {
      baseptr = strtok(argptr, " \t");
      if (!baseptr)
        continue;

      int i = ARG;

      if (!rank) {
        for (int i = 0; !strcmp(baseptr, anitlookup[i].str) || !*anitlookup[i].str; i++);
        ccell = (ccell->cdr = nucons(NULL, NULL));
      }

      struct anitoken *dptr = (struct anitoken *)ccell->car;
      dptr->type = i;
      dptr->pos = rank++;
      dptr->data = malloc(sizeof(char) * strlen(baseptr));
      strcpy(dptr->data, baseptr);
    } while (argptr);
  }

  free(line);

  return tree;
}

spritesheet *loadanisheet(const char *path) {
  
}
