#include "anidata.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3_image/SDL_image.h>

#include "bsd.h"
#include "alias_t.h"

spritesheet *loadsheet(SDL_Renderer *ren, const char *path) {
  const char *fullpath = elfypath(path);
  SDL_Texture *tex = IMG_LoadTexture(ren, fullpath);
  free(fullpath);

  if (!tex)
    return NULL;

  spritesheet *ret = malloc(sizeof(spritesheet));
  ret->refcount = 0;
  ret->texture = tex;
  
  return ret;
}

void destroyspritesheet(spritesheet *sheet) {
  SDL_DestroyTexture(sheet->texture);
  free(sheet);
}

int destroyanidata(anidata *data) {
  spritesheet *sheet = data->sheet;

  free(data);
  
  if (--sheet->refcount <= 0) {
    destroyspritesheet(sheet);
    return 0;
  }

  return sheet->refcount;
}

SDL_FRect modrect(const SDL_FRect *rect, const float xmul, const float ymul) {
  return (SDL_FRect) { rect->x + rect->w * xmul, rect->y + rect->h * ymul,
		       rect->w, rect->h};
}

SDL_FRect transrect(const SDL_FRect *rect, const float x, const float y) {
  return (SDL_FRect) { x, y, rect->w, rect->h};
}

bool drawframe(SDL_Renderer *ren, anidata *data, const float x,
	       const float y, const uint32 delta) {
  if ((data->ticks += delta) >= data->delay) {
    data->cframe += data->ticks / data->delay;
    data->ticks %= data->delay;

    if (data->cframe >= data->frames)
      data->cframe %= data->frames;
  }

  SDL_FRect srcr = modrect(&data->rect, data->cframe % data->cols, data->cframe / data->cols);
  SDL_FRect dstr = transrect(&data->rect, x, y);

  return SDL_RenderTexture(ren, data->sheet->texture, &srcr, &dstr);
}

bool drawaframe(SDL_Renderer *ren, anidata *data, const float x,
	       const float y, const uint32 frame) {

  SDL_FRect srcr = modrect(&data->rect,
			   frame >= data->frames ? (data->frames - 1) % data->cols : frame % data->cols,
			   frame >= data->frames ? (data->frames - 1) / data->cols : frame / data->cols);
  SDL_FRect dstr = transrect(&data->rect, x, y);

  return SDL_RenderTexture(ren, data->sheet->texture, &srcr, &dstr);
}
