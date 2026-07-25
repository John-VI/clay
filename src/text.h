#ifndef CLAY_TEXT
#define CLAY_TEXT

#include <SDL3/SDL_render.h>

#include "anidata.h"

bool scrputc(SDL_Renderer *, anidata *, const char, const float, const float);
bool scrputs(SDL_Renderer *, anidata *, const char *, const float, const float);

#endif
