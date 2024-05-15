#pragma once

#include "types.h"

#define LIMEFONT_MAGIC "LIMEFONT"

typedef struct {
  char magic[8];
  u32 glyphs;
  u32 relocs;
  u16 width;
  u16 height;
} limefont_header_t;

typedef struct {
  u32 source;
  u32 target;
  u32 count;
} limefont_relocation_t;

BOOL limefont_parse(void *font);
