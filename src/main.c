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
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_stdinc.h>

#include "aniloader.h"
#include "anidata.h"
#include "text.h"
#include "cons.h"

struct flyins { // This should be its own source file but it's just not worth
                // pushing out now.
  signed char lr;
  signed char ud;
  float x;
  float y;
  char c;
};

int main(int argc, char *argv[]) {
  /*** EPIGENICS ***/
  const SDL_InitFlags basesubsystems =
    SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS;
  
  const char	 progtitle[] = "Uncharacterized Clay";
  const char	 progid[]    = "CLAY1";

  SDL_Window	*win;
  SDL_Renderer	*ren;
  SDL_Event	 e;

  const bool	*kbd;

  spritesheet	*frog;
  spritesheet	*compac;
  spritesheet   *bg;

  cons		*sdeck	     = NULL;

  uint64	 prevticks   = 0;
  uint64         ticks       = 0;
  int		 bgstate     = 0;
  uint64         spawndelay  = 0;
  char		 fps[5]	     = "XX";
  char           lstat[250]  = "";

  int freecount = 0;

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

  kbd = SDL_GetKeyboardState(NULL);
  bg = loadanisheet("dirtest.anis", ren);

  printf("Startup took %llums.\n", SDL_GetTicks());
  prevticks = SDL_GetTicks();

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
      for (i = 24; ((*(uint32 *)(kbd + SDL_SCANCODE_RIGHT))>>i & 255) == 0
	     && i >= 0; i -= 8);
      bgstate = (i / 8) + 1;
    }

    if ((spawndelay += ticks) >= 200) {
      signed char clr = SDL_rand(3) - 1;
      signed char cud = SDL_rand(3) - 1;

      if (clr || cud) {
	struct flyins *fly = malloc(sizeof(struct flyins));
	fly->lr = clr;
	fly->ud = cud;

	char coin = SDL_rand(4);
	switch (coin) {
	case 0:
	  fly->x = 0;
	  fly->y = SDL_rand(600);
	  break;
	case 1:
	  fly->x = 800;
	  fly->y = SDL_rand(600);
	  break;
	case 2:
	  fly->x = SDL_rand(800);
	  fly->y = 0;
	  break;
	case 3:
	  fly->x = SDL_rand(800);
	  fly->y = 600;
	  break;
	}

	fly->c = SDL_rand(26);
	listpush(&sdeck, fly);
      }

      spawndelay -= 500;
    }

    cons *ccons = sdeck;
    cons *pcons = NULL;
    while (ccons) {
      struct flyins *fly = ccons->car;
      if (fly->x < 0.0f || fly->x > 800.0f
	  || fly->y < 0.0f || fly->y > 600.0f) {
	free(delcons(pcons, &ccons));
	if (!pcons)
	  sdeck = ccons;
	freecount++;
      } else {
	fly->x += .1 * ticks * fly->lr;
	fly->y += .1 * ticks * fly->ud;
	pcons = ccons;
	ccons = ccons->cdr;
      }
    }

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    SDL_RenderTexture(ren, frog->texture, NULL, NULL);

    drawaframe(ren, frog->anis, 0, 0, 0);
    drawaframe(ren, frog->anis+1, 0, 0, 0);

    drawaframe(ren, bg->anis, 0, 0, bgstate);

    ccons = sdeck;
    while (ccons) {
      struct flyins *fly = ccons->car;
      drawaframe(ren, compac->anis, (int)fly->x, (int)fly->y, fly->c);
      ccons = ccons->cdr;
    }
    
    scrputs(ren, compac->anis, lstat, 0, 16);
    scrputs(ren, compac->anis, fps, 0, 0);
    
    SDL_RenderPresent(ren);

    snprintf(fps, 5, "%llu", (prevticks - SDL_GetTicks()) / 1000);
    snprintf(lstat, 250, "sdeck len %u, sd %llu\nticks %llu, %d", listlen(sdeck), spawndelay, ticks, freecount);

    ticks = SDL_GetTicks() - prevticks;
    prevticks = SDL_GetTicks();
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
