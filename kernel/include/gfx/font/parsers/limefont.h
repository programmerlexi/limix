#pragma once

#include <stdbool.h>
#include <stdint.h>

#define LIMEFONT_MAGIC "LIMEFONT"

typedef struct {
  char magic[8];
  uint64_t glyphs;
  uint64_t relocs;
  uint32_t width;
  uint32_t height;
} limefont_header_t;
bool limefont_parse(void *font);
