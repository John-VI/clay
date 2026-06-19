#include "text.h"

#include <SDL3/SDL_render.h>

#include "anidata.h"

bool scrputc(SDL_Renderer *ren, anidata *data, const char c, const float x, const float y) {
  return drawaframe(ren, data, x, y, c - ' ');
}

bool scrputs(SDL_Renderer *ren, anidata *data, const char *s, const float x, const float y) {
  for (int i = 0; s[i]; ++i)
    if (!scrputc(ren, data, s[i], x + data->rect.w * i, y))
      return false;
  return true;
}
