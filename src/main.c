#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_filesystem.h>

#include "aniloader.h"
#include "anidata.h"
#include "text.h"

int main(int argc, char *argv[]) {
  /*** EPIGENICS ***/
  const SDL_InitFlags basesubsystems =
    SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS;
  
  const char	 progtitle[] = "Uncharacterized Clay";
  const char	 progid[]    = "CLAY1";

  SDL_Window	*win;
  SDL_Renderer	*ren;
  SDL_Event	 e;

  spritesheet *frog;
  spritesheet	*compac;
  anidata	*font;

  SDL_SetHint(SDL_HINT_APP_NAME, progtitle);
  SDL_SetHint(SDL_HINT_APP_ID, progid);
  
  /*** BIRTH ***/
  bool sdlerr = !SDL_Init(basesubsystems);
  if (sdlerr) {
    fprintf(stderr, "Birth crash!\nSDL_GetError():\n%s\n", SDL_GetError());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Clay Fatal Error!",
			     SDL_GetError(), NULL);
    return 1;
  }

  sdlerr = !SDL_CreateWindowAndRenderer(progtitle, 800, 600,
				       0, &win, &ren);
  if (sdlerr) {
    fprintf(stderr, "Birth crash!\nSDL_GetError():\n%s\n", SDL_GetError());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Clay Fatal Error!",
			     SDL_GetError(), NULL);
    return 1;
  }

  // frogfullpathlen = strlen(SDL_GetBasePath()) + strlen("hey.png") + 1;
  // frogfullpath = malloc(frogfullpathlen);
  // strlcpy(frogfullpath, SDL_GetBasePath(), frogfullpathlen);
  // strlcat(frogfullpath, "hey.png", frogfullpathlen);
  // frog = IMG_LoadTexture(ren, frogfullpath);
  // free(frogfullpath);

  frog = loadanisheet("test.anis", ren);
  if (!frog) {
    fprintf(stderr,
            "CRITICAL FROG ERROR!\nSDL_GetError():\n%s\nloadererrstr():\n%s\n",
            SDL_GetError(), loadererrstr());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CRITICAL FROG ERROR!",
                             SDL_GetError(), NULL);
    return 1;
  }

  compac = loadsheet(ren, "compac.png");
  if (!compac) {
    fprintf(stderr, "compac.png could not be loaded!\nSDL_GetError():\n%s\n", SDL_GetError());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Spritesheet load error!",
			     SDL_GetError(), NULL);
    return 1;
  }

  font = malloc(sizeof(anidata));
  font->name   = "Compac";
  font->frames = 96;
  font->cols   = 96;
  font->rect.x = 0;
  font->rect.y = 0;
  font->rect.w = 8;
  font->rect.h = 16;
  font->flags  = 0;
  font->delay  = 500;
  // font->ticks  = 0;
  // font->cframe = 0;
  font->sheet  = compac;
  compac->refcount++;

  /*** LIFE ***/
  while (1) {
    SDL_PollEvent(&e);
    if (e.type == SDL_EVENT_QUIT) {
      break;
    }

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    SDL_RenderTexture(ren, frog->texture, NULL, NULL);

    drawaframe(ren, frog->anis, 0, 0, 0);
    drawaframe(ren, frog->anis+1, 0, 0, 0);
    
    scrputs(ren, font, "\nText processing.\n", 0, 16);
    for (char i = 0; i < 96; i++)
      scrputc(ren, font, i + ' ', i * 8, 32);
    
    SDL_RenderPresent(ren);
  }

  /*** DEATH ***/
  unlinksheet(frog);
  // destroyanidata(font);
  
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  
  SDL_QuitSubSystem(basesubsystems);
  SDL_Quit();
  
  return 0;
}
