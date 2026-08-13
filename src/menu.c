#include "menu.h"

#include "cons.h"

#include <string.h>

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
    menu *menuptr;
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

static int stackusage = -1;
static menu menustack[10];
static int stackpath[10] = { 0 };

void initmenu(int basemenu) {
  if (stackusage == -1) {
    memset(menustack, NULL, sizeof(menu) * 10);
    stackusage = 0;
  }

  switch (basemenu) {
  case 0: // Main Menu
    menustack[0].len = 3;
    menustack[0].title = "Main Menu";
    
  }
}
