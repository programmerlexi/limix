#pragma once

#include <stdint.h>

typedef union vt_color {
  struct {
    uint8_t reserved;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  } ccolor;
  uint32_t fb_color;
} vt_color_t;

typedef struct vt_graphic_rendition {
  vt_color_t fg_rgb; /* Incompatible with bold/dim */
  uint8_t fg_index;

  vt_color_t bg_rgb;
  uint8_t bg_index;

  uint8_t font_state;
} vt_graphic_rendition_t;

typedef struct vt_char {
  uint32_t unicode;
  vt_color_t fg;
  vt_color_t bg;
} vt_char_t;

#define VT_BOLD 1
#define VT_DIM 2
#define VT_REVERSE 4

void vt_init();
void vt_clear();
void vt_flush();
void kprintc(char);
void kprint(char *);
