#include "menu.h"

#include <stdlib.h>
#include <stdbool.h>

#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_render.h>

#include "text.h"
#include "anidata.h"

typedef enum moptype {
  DUMMY,
  TEXT,
  SUBMENU,
  STEP,
  TEXTENTRY,
  KEYBIND,
  BACK,
  QUIT,
  MOPTYPEMAX
} moptype;

typedef struct menu menu;

typedef union menuact {
  moptype type;

  struct {
    moptype type;
    int mindex;
  } submenu;

  struct {
    moptype type;
    int step;
    void (*stepfunc)(int);
  } step;
} menuact;

typedef struct menuopt {
  char		 flags;
  char		*text;
  menuact	 act;
} menuopt;

typedef struct menu {
  int		 len;
  char		*title;
  menuopt	opts[50];
} menu;

static const menu *menustack;
static int stackpathusage = 0;
static int stackpath[10] = { -1 };
static int cmenu = 0;
static int sel = 0;

static const uint64 tdelay = 300;
static uint64 ticks = tdelay;
SDL_FPoint cursorpos = { 0, 0 };
SDL_FPoint newpos = { 0, 0 };

static const menu mainmenu[10] = {
  { 3, "Main Menu",
    {
      { 0, "Resume", { BACK } },
      { 0, "Options", { .submenu = { SUBMENU, 1 } } },
      { 0, "Quit", { QUIT } }
    }
  },
  { 1, "Options", { { 0, "Back", { BACK } } } },
};

void initmenu(int basemenu) {
  // if (stackusage == -1) {
  //   memset(menustack, NULL, sizeof(menu) * 10);
  //   stackusage = 0;
  // }

  switch (basemenu) {
  case 0: // Main Menu
    menustack = mainmenu;
    // menustack[0].len = 3;
    // menustack[0].title = "Main Menu";

    // menustack[0].opts[0].text = "Resume";
    // menustack[0].opts[0].act.type = BACK;

    // menustack[0].opts[1].text = "Options";
    // menustack[0].opts[1].act.type = SUBMENU;
    // menustack[0].opts[1].act.submenu.mindex = 1;

    // menustack[0].opts[2].text = "Exit to Windows";
    // menustack[0].opts[2].act.type = QUIT;

    // menustack[1].len = 1;
    // menustack[1].title = "Options";

    // menustack[1].opts[0].text = "Back";
    // menustack[1].opts[0].act.type = BACK;

    stackpath[0] = 0;
    stackpathusage = 0;
    cmenu = 0;
    break;
  }
}

bool menup() {
  return stackpath[0] >= 0;
}

void menuevent(SDL_Scancode key) {
  // if (key == lastkey && (ticks += t) < tdelay)
  //   return;

  switch (key) {
  case SDL_SCANCODE_UP:
    if (--sel < 0)
      sel = menustack[cmenu].len - 1;
    break;

  case SDL_SCANCODE_DOWN:
    if (++sel >= menustack[cmenu].len)
      sel = 0;
    break;

  case SDL_SCANCODE_RETURN:
  tickmenu_activate:
    switch (menustack[cmenu].opts[sel].act.type) {
    case SUBMENU:
      cmenu = menustack[cmenu].opts[sel].act.submenu.mindex;
      sel = 0;
      stackpath[++stackpathusage] = cmenu;
      break;

    case STEP:
      menustack[cmenu].opts[sel].act.step.stepfunc(
          menustack[cmenu].opts[sel].act.step.step);
      break;

    case BACK:
    tickmenu_back:
      if (stackpathusage) {
      stackpath[stackpathusage--] = -1;
      cmenu = stackpath[stackpathusage];
      sel = 0;
      } else {
        stackpath[0] = -1;
      }
      break;

    case QUIT:;
      SDL_QuitEvent *q =
          malloc(sizeof(SDL_QuitEvent)); // Quitter. Kill yourself.
      q->type = SDL_EVENT_QUIT;
      q->timestamp = SDL_GetTicksNS();

      SDL_PushEvent((SDL_Event *)q);

    default:
      break;
    }
    break;

  case SDL_SCANCODE_RIGHT:
    goto tickmenu_activate;
    break; // I'm paranoid.

  case SDL_SCANCODE_LEFT:
    if (menustack[cmenu].opts[sel].act.type == STEP)
      menustack[cmenu].opts[sel].act.step.stepfunc(
          -menustack[cmenu].opts[sel].act.step.step);
    else
      goto tickmenu_back;
    break;

  case SDL_SCANCODE_ESCAPE:
    goto tickmenu_back;
    break;

  default:
    return;
  }

  // ticks = 0;
  // lastkey = key;
  return;
}

void drawmenu(SDL_Renderer *ren, spritesheet *font) {
  for (int i = 0; i <= stackpathusage; i++) {
    scrputs(ren, font->anis, menustack[stackpath[i]].title, 16 + i * 200, 200);

    for (int j = 0; j < menustack[stackpath[i]].len; j++)
      scrputs(ren, font->anis, menustack[stackpath[i]].opts[j].text,
        24 + i * 200, 200 + 16 * (j + 1));
  }
  scrputc(ren, font->anis, '>', stackpathusage * 200, 200 + 16 * (sel+1));
}
