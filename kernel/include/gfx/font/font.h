#pragma once

#include <stdint.h>
#define LIMEFONT_MAGIC "LIMEFONT"

typedef struct {
  char magic[8];
  uint64_t glyphs;
  uint64_t relocs;
  uint32_t width;
  uint32_t height;
} lime_font_header_t;

void font_parse();

extern uint8_t *g_8x16_font;
