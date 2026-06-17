#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  /*** EPIGENICS ***/
  const SDL_InitFlags basesubsystems = SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS;
  const char progtitle[] = "Uncharacterized Clay";
  const char progid[] = "CLAY1";

  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Event e;

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

  /*** LIFE ***/
  while (1) {
    SDL_PollEvent(&e);
    if (e.type == SDL_EVENT_QUIT) {
      break;
    }
    
  }

  /*** DEATH ***/
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  
  SDL_QuitSubSystem(basesubsystems);
  SDL_Quit();
  
  return 0;
}
