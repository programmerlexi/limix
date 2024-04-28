#pragma once

#include <stdbool.h>
#include <stdint.h>

#define LIMEFONT_MAGIC "LIMEFONT"

typedef struct {
  char magic[8];
  uint32_t glyphs;
  uint32_t relocs;
  uint16_t width;
  uint16_t height;
} limefont_header_t;

typedef struct {
  uint32_t source;
  uint32_t target;
  uint32_t count;
} limefont_relocation_t;

bool limefont_parse(void *font);
