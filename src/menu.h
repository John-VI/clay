#ifndef CLAY_MENU
#define CLAY_MENU

#include "alias_t.h"
#include "anidata.h"

#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_render.h>

void initmenu(int);
void menuevent(SDL_Scancode);
void tickmenu(uint64);
void drawmenu(SDL_Renderer *, spritesheet *);

bool menup();

#endif
