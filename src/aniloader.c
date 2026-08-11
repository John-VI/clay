#include "aniloader.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDL3_image/SDL_image.h>

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
  enum aniptype	 type;
  uint32	 pos;
  char		*data;
};

struct aniextok {
  const char		*str;
  const uint32		 args;
  const enum argtype	 argtype;
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

static SDL_TLSID errint = { 0 };

const char *loadererrstr() {
  return (const char *)SDL_GetTLS(&errint);
}

void seterr(const char *str) {
  SDL_SetTLS(&errint, str, NULL);
}

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
    cons *ccell = listpush(&tree, NULL);
    uint32 rank = 0;

    do {
      baseptr = strsep(&argptr, " \t");
      if (!*baseptr)
        continue;

      int i = ARG;

      if (!rank) {
        for (i = 0; strcmp(baseptr, anitlookup[i].str) && i <= BAD; i++);
        ccell = (ccell->car = nucons(malloc(sizeof(struct anitoken)), NULL));
      } else {
        ccell = listpush(&ccell, malloc(sizeof(struct anitoken)));
      }

      struct anitoken *dptr = (struct anitoken *)ccell->car;
      dptr->type = i;
      dptr->pos = rank++;
      dptr->data = malloc(sizeof(char) * (strlen(baseptr) + 1));
      strcpy(dptr->data, baseptr);
    } while (argptr);
  }
  free(line);

  return tree;
}

void destroytree(cons *tree) {
  cons *ccell = tree;
  while (ccell) {
    cons *subcell = ccell->car;
    while (subcell) {
      free(((struct anitoken *)subcell->car)->data);
      free(subcell->car);
      cons *lcl = subcell;
      subcell = subcell->cdr;
      free(lcl);
    }
    cons *lcl = ccell;
    ccell = ccell->cdr;
    free(lcl);
  }
}

spritesheet *loadanisheet(const char *path, SDL_Renderer *ren) {
  FILE *f; // Closed in all paths
  cons *tree; // Destroyed in all paths
  unsigned int llen = 0;
  struct spritesheet *sheet = NULL;
  char *texturepath = NULL; // Freed in all paths

  if (!(f = fopen(path, "r"))) {
    seterr("File didn't open.");
    return NULL; // TODO: Handle error BETTER
  }

  tree = ingestdata(f);
  fclose(f);
  if (!tree) {
    seterr("The woods burnt down.");
    return NULL;
  }
  
  for (cons *ccell = tree; ccell; ccell = ccell->cdr) {
    if (((struct anitoken *)((cons *)ccell->car)->car)->type == BAD) {
      seterr("Invalid field.");
      goto loadanisheet_err;
    }
    llen++;
  }

  if ((llen - 2) % 6) {
    seterr("Bad line count.");
    goto loadanisheet_err;
  }

  sheet = calloc(1, sizeof(spritesheet));
  sheet->anis = calloc((llen - 2) / 6, sizeof(struct anidata));
  sheet->anicount = (llen - 2) / 6;
  sheet->texture = NULL;

  enum aniptype checker = 0;
  cons *bcell = tree;
  anidata *cdata = sheet->anis;

  while (bcell) {
    cons *ccell = bcell->car;
    enum aniptype type = ((struct anitoken *)ccell->car)->type;

    if (checker & 1 << type) {
      seterr("File malformed.");
      goto loadanisheet_err;
    }

    if (anitlookup[type].args && !ccell->cdr) {
      seterr("Token that should have had an argument didn't.");
      goto loadanisheet_err;
    }

    switch (type) {
      case SHTTEXT:
        texturepath = ((struct anitoken *)ccell->cdr->car)->data;
        ((struct anitoken *)ccell->cdr->car)->data = NULL;
        break;
      case SHTNAME:
        sheet->name = ((struct anitoken *)ccell->cdr->car)->data;
        ((struct anitoken *)ccell->cdr->car)->data = NULL;
        break;

      case DATNAME:
        cdata->name = ((struct anitoken *)ccell->cdr->car)->data;
        ((struct anitoken *)ccell->cdr->car)->data = NULL;
        break;

      case DATFRMS:
        cdata->frames =
            strtoul(((struct anitoken *)ccell->cdr->car)->data, NULL, 10);
        break;

      case DATCOLS:
        cdata->cols =
            strtoul(((struct anitoken *)ccell->cdr->car)->data, NULL, 10);
        if (!cdata->cols) {
          seterr("Zero columns for data?");
          goto loadanisheet_err;
        }
        break;

      case DATDLAY:
        cdata->delay =
            strtoul(((struct anitoken *)ccell->cdr->car)->data, NULL, 10);
        break;

      case DATFLAG:
        cdata->flags =
            strtoul(((struct anitoken *)ccell->cdr->car)->data, NULL, 10);
        break;

      case DATRECT:
        for (int i = 0; i < anitlookup[DATRECT].args; i++)
          if (listlen(ccell->cdr) < 4) {
            seterr("Rectangle argument underrun.");
            goto loadanisheet_err;
          }
        if (listlen(ccell->cdr) > 4) {
          seterr("Rectangle argument overrun.");
          goto loadanisheet_err;
        } //Bein lazy. Optimize this. TODO
        
        cdata->rect.x = (float)strtol(
            (((struct anitoken *)nth(ccell, 1)->car)->data), NULL, 10);
        cdata->rect.y = (float)strtol(
            (((struct anitoken *)nth(ccell, 2)->car)->data), NULL, 10);
        cdata->rect.w = (float)strtol(
            (((struct anitoken *)nth(ccell, 3)->car)->data), NULL, 10);
        cdata->rect.h = (float)strtol(
            (((struct anitoken *)nth(ccell, 4)->car)->data), NULL, 10);
        break;

      default:
        seterr("Invalid token in the tree. What?");
        goto loadanisheet_err; // What the!?
      }

      checker |= 1<<type;
      if (checker>>2 == 63) {
        checker ^= 63<<2;
        cdata->sheet = sheet;
        cdata++;
      }

      bcell = bcell->cdr;
  }

  if (checker != 0b11) {
    seterr(checker > 0b11 ? "Dangling data attributes." : "Sheet attributes missing.");
    goto loadanisheet_err;
  }

  char textpath[500];
  strlcpy(textpath, SDL_GetBasePath() ? SDL_GetBasePath() : "./", 500);
  strlcat(textpath, texturepath, 500);
  free(texturepath);

  sheet->texture = IMG_LoadTexture(ren, textpath);
  if (!sheet->texture) {
    seterr("Diagnosis: SDL bullshit.");
    goto loadanisheet_err;
  }
  sheet->refcount = 1;

  destroytree(tree);
    
  return sheet;

loadanisheet_err:
  destroytree(tree);
  free(texturepath);

  if (sheet) {
    sheet->refcount = 0;

    for (int i = 0; i < sheet->anicount; i++)
      free(sheet->anis[i].name);
    free(sheet->anis);

    if (sheet->name)
      free(sheet->name);

    if (sheet->texture)
      SDL_DestroyTexture(sheet->texture);

    free(sheet);
  }

  /*** Failure cases that must be covered:
    [ ] Only list allocated
    [ ] List & sheet without texture (sheet->anis at variable loading state)
    [ ] List & full sheet
  ***/

  return NULL;
} 
