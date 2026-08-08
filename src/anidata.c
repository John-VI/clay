#include "anidata.h"

#include <stdlib.h>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3_image/SDL_image.h>

#include "alias_t.h"

void destroyspritesheet(spritesheet *sheet) {
  SDL_DestroyTexture(sheet->texture);
  free(sheet);
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
  // if ((data->ticks += delta) >= data->delay) {
  //   data->cframe += data->ticks / data->delay;
  //   data->ticks %= data->delay;

  //   if (data->cframe >= data->frames)
  //     data->cframe %= data->frames;
  // }

  SDL_FRect srcr = modrect(&data->rect, delta % data->cols, delta / data->cols);
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

int unlinksheet(spritesheet *sheet) {
  if (--(sheet->refcount) <= 0) {
    for (int i = 0; i < sheet->anicount; i++)
      free(sheet->anis[i].name);
    free(sheet->anis);
    free(sheet->name);
    SDL_DestroyTexture(sheet->texture);
    free(sheet);
    return 0;
  } else {
    return sheet->refcount;
  }
}
