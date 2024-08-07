#pragma once

#include "libk/types.h"
#include <stdbool.h>

#define LIMEFONT_MAGIC "LIMEFONT"

typedef struct {
  char magic[8];
  u32 glyphs;
  u32 relocs;
  u16 width;
  u16 height;
} LimefontHeader;

typedef struct {
  u32 source;
  u32 target;
  u32 count;
} LimefontRelocation;

bool limefont_parse(void *font);
