//#include "aniloader.h"

#include <stdio.h>

#include "anidata.h"
#include "cons.h"
#include "alias_t.h"
#include "bsd.h"

enum aniptype {
  UNPARSE,
  SHTNAME,
  SHTTEXT,

  DATNAME,
  DATFRMS,
  DATCOLS,
  DATRECT,
  DATFLAG,
  DATDLAY,
  BAD
};

enum argtype {
  INT,
  UINT,
  STR,
  NIL
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

const struct aniextok anitlookup[BAD+1] = {
  { "",             0, NIL  },
  { "sheetname",    1, STR  },
  { "sheettexture", 1, STR  },

  { "name",         1, STR  },
  { "frames",       1, UINT },
  { "columns",      1, UINT },
  { "rectangle",    4, INT  },
  { "flags",        1, NIL  },
  { "delay",        1, UINT },

  { "",             0, NIL  }
};

cons *ingestdata(FILE *f) {
  cons *tree = NULL;
  char *line = NULL;
  size_t buflen = 0;
  int len = 0;

  while ((len = getline(&line, &buflen, f)) != -1) {
    
  }
}
