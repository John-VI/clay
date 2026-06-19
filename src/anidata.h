#ifndef CLAY_ANIDATA
#define CLAY_ANIDATA

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include "alias_t.h"

struct spritesheet {
  SDL_Texture *texture;
  int refcount;
};

typedef struct spritesheet spritesheet;

struct anidata {
  /* const */ char	*name;
  /* const */ uint32	 frames;
  /* const */ SDL_FRect	 rect;
  /* const */ char	 flags;
  /* const */ uint32	 delay;

  uint32		 ticks;
  uint32		 cframe;
  spritesheet		*sheet;
};

typedef struct anidata anidata;

spritesheet *loadsheet(SDL_Renderer *, const char *);

bool drawframe(SDL_Renderer *, anidata *, const float, const float,
	       const uint32);

int destroyanidata(anidata *);

// destroyspritesheet() is hidden since destroyanidata should do it itself.

#endif 
