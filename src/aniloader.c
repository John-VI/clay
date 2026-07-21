#include "aniloader.h"

#include <stdio.h>
#include <threads.h>
#include <stdlib.h>

#include <SDL3/SDL_filesystem.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_render.h>

#include "bsd.h"
#include "anidata.h"
#include "alias_t.h"

enum uint32 loader_error {
  LOADER_NOPROB,
  LOADER_FILENOTFOUND,
  LOADER_PERMISSIONDENIED,
  LOADER_FACEMALFORMED,
  LOADER_BUSTMALFORMED,
  LOADER_WHAT
};

enum bustflags {
  BNAME   = 1<<0,
  BFRAMES = 1<<1,
  BCOLS   = 1<<2,
  BRECT   = 1<<3,
  BFLAGS  = 1<<4,
  BDELAY  = 1<<5,
  BERR    = 1<<6
};

enum faceflags {
  SHEETNAME = 1<<0,
  TEXTURE   = 1<<1,
  LOADERR   = 1<<2
};

static SDL_TLSID loaderr = 0;

static const char *loader_errortext[] = {
  "No problem!",
  "File not found.",
  "Permission denied.",
  "File face is malformed.",
  "File bust is malformed.",
  "An error not otherwise described occurred."
};

const int loadererrint() { return SDL_GetTLS(&loaderr); }
const char *loadererrstr() { return loader_errortext[SDL_GetTLS(&loaderr)]; }
void loaderclrerr() { SDL_SetTLS(&loaderr, &LOADER_NOPROB, NULL); }

// This one is just for us.
void loaderseterr(loader_error err) { SDL_SetTLS(&loaderr, &err, NULL); }

bool ani_procface(FILE *f, spritesheet *sheet) {
  faceflags check = 0;

  char *line = NULL;
  getline(&line, NULL, f);

  while (!feof(f)) {
    if (strlen(line) > 1) {
      char *chomped = chomp(line);
      free(line);
      line = NULL
      char *args = chomped;
      
      strsep(&args, " \t");

      if (strcmp(chomped, "sheetname") == 0) {
	if (check & SHEETNAME) {
	  free(sheet->name);
	  if (check & TEXTURE)
	    SDL_DestroyTexture(sheet->texture);
	  checkflags = LOADERR;
	}
	
	char *name = chomp(args);
	if (strlen(name)) {
	  sheet->name = name;
	  check |= SHEETNAME;
	} else {
	  free(name);
	  check = LOADERR;
	}
      } else if (strcmp(chomped, "texture") == 0) {
	if (check & TEXTURE) {
	  if (check & SHEETNAME)
	    free(check->name);
	  SDL_DestroyTexture(sheet->texture);
	  check = LOADERR;
	}
	
	char *name = chomp(args);
	int res = 0;
	SDL_free(SDL_GlobDirectory(SDL_GetBasePath(), name, &res));

	if (strlen(name) && res == 1) {
	  char *path = elfypath(name);
	  sheet->texture = IMG_Load(path);
	  free(path);
	  if (!sheet->texture) {
	    if (check & SHEETNAME)
	      free(sheet->name);
	    check = LOADERR;
	  }
	} else {
	  if (check & SHEETNAME)
	    free(sheet->name);
	  check = LOADERR;
	}
	free(name);
      }

      free(chomped);
    }

    if (check & LOADERR) {
      loaderseterr(LOADER_FACEMALFORMED);
      return false;
    } else if (check & SHEETNAME & TEXTURE) {
      return true;
    }
  }
  
  if (check & SHEETNAME)
    free(sheet->name);
  else if (check & TEXTURE)
    SDL_DestroyTexture(sheet->texture);
  
  loaderseterr(LOADER_FACEMALFORMED);
  return false;
}

/* void procbust_handleerr(const uint32 flags, anidata *data) { */
/*   uint32 i = 1; */
/*   do { */
/*     switch (flags & i) { */
/*     case BNAME: */
/*       free(data->name); */
/*       continue; */
/*     } */
/*   } */
/* } */

bool ani_procbust(FILE *f, anidata *data) {
  bustflags check = 0;

  while (check != BERR - 1) {
    if (feof(f)) {
      if (!check)
	break;
      else {
	check |= BERR;
	goto ani_procbust_error;
      }
    }
    
    char *line = NULL;
    getline(&line, NULL, f);
    
    if (strspn(line, " \t\n\r") != strlen(line)) {

      char *chomped = chomp(line);
      char *args = chomped;
      strsep(&args, " \t");

      if (!strcmp(chomped, "name"))
	if (!(check & BNAME)) {
	  data->name = strcpy(malloc(strlen(args)+1), args);
	  check |= BNAME;
	}
	else
	  check |= BERR;

      else if (!strcmp(chomped, "frames"))
	if (!(check & BFRAMES)) {
	  data->frames = (uint32)strtoul(args, NULL, 10);
	  check |= BFRAMES;
	} else
	  check |= BERR;

      else if (!strcmp(chomped, "cols"))
	if (!(check & BCOLS)) {
	  data->cols = (uint32)strtoul(args, NULL, 10);
	  check |= BCOLS;
	} else
	  check |= BERR;
      
      else if (!strcmp(chomped, "flags"))
	if (!(check & BFLAGS)) {
	  data->flags = (char)strtoul(args, NULL, 10);
	  check |= BFLAGS;
	} else
	  check |= BERR;
    
      else if (!strcmp(chomped, "delay"))
	if (!(check & BDELAY)) {
	  data->delay = (uint32)strtoul(args, NULL, 10);
	  check |= BDELAY;
	} else
	  check |= BERR;
      else
	check |= BERR;

      free(chomped);

      if (check & BERR) {
      ani_procbust_error:
	
	if (check & BNAME) {
	  free(data->name);
	  check ^= BNAME;
	}
	loaderseterr(LOADER_BUSTMALFORMED);
	break;
      } else {
	free(line);
	line = NULL;
	getline(&line, NULL, f);
      }
    }
    free(line);
  }

  if (!check || loadererrint())
    return false;
  else
    return true;
}
