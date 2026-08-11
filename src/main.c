#include <SDL3/SDL_scancode.h>
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
#include <SDL3/SDL_keyboard.h>

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

  spritesheet	*frog;
  spritesheet	*compac;
  spritesheet   *bg;

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

  frog = loadanisheet("test.anis", ren);
  if (!frog) {
    fprintf(stderr,
            "CRITICAL FROG ERROR!\nSDL_GetError():\n%s\nloadererrstr():\n%s\n",
            SDL_GetError(), loadererrstr());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CRITICAL FROG ERROR!",
                             SDL_GetError(), NULL);
    return 1;
  }

  compac = loadanisheet("compac.anis", ren);
  if (!compac) {
    fprintf(stderr,
            "compac.png could not be "
            "loaded!\nSDL_GetError():\n%s\nloadererrstr():\n%s\n",
            SDL_GetError(), loadererrstr());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Spritesheet load error!",
			     SDL_GetError(), NULL);
    return 1;
  }

  const bool *kbd = SDL_GetKeyboardState(NULL);
  int bgstate = 0;
  bg = loadanisheet("dirtest.anis", ren);

  /*** LIFE ***/
  while (1) {
    SDL_PollEvent(&e);
    if (e.type == SDL_EVENT_QUIT) {
      break;
    }

    if (kbd[SDL_SCANCODE_SPACE])
      bgstate = 0;
    else if ((*(uint32 *)(kbd + SDL_SCANCODE_RIGHT))) {
      int i;
      for (i = 24; ((*(uint32 *)(kbd + SDL_SCANCODE_RIGHT))>>i & 255) == 0 && i >= 0; i -= 8);
      bgstate = (i / 8) + 1;
    }

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    SDL_RenderTexture(ren, frog->texture, NULL, NULL);

    drawaframe(ren, frog->anis, 0, 0, 0);
    drawaframe(ren, frog->anis+1, 0, 0, 0);

    drawaframe(ren, bg->anis, 0, 0, bgstate);
    
    scrputs(ren, compac->anis, "\nText processing.\n", 0, 16);
    for (char i = 0; i < 96; i++)
      scrputc(ren, compac->anis, i + ' ', i * 8, 32);
    
    SDL_RenderPresent(ren);
  }

  /*** DEATH ***/
  unlinksheet(frog);
  unlinksheet(compac);
  
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  
  SDL_QuitSubSystem(basesubsystems);
  SDL_Quit();
  
  return 0;
}
