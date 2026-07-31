#ifndef CLAY_ANILOADER
#define CLAY_ANILOADER

//#include <stdio.h>

#include <SDL3/SDL_render.h>

#include "anidata.h"
#include "alias_t.h"

uint32 loadererrint();
const char *loadererrstr();
void loaderclrerr();

spritesheet *loadanisheet(const char *fullpath, SDL_Renderer *ren);

#endif
