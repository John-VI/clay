#ifndef CLAY_ANILOADER
#define CLAY_ANILOADER

#include <stdio.h>

#include "anidata.h"
#include "alias_t.h"

const uint32 loadererrint();
const char *loadererrstr();
void loaderclrerr();

int ani_readfile(const char *);
bool ani_procface(FILE *, spritesheet *);
bool ani_procbust(FILE *, anidata *);

#endif
