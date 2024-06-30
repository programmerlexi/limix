#pragma once

#include "libk/types.h"
#include <stdbool.h>

typedef union vt_color {
  struct {
    u8 reserved;
    u8 red;
    u8 green;
    u8 blue;
  } ccolor;
  u32 fb_color;
} vt_color_t;

typedef struct vt_graphic_rendition {
  vt_color_t fg_rgb; /* Incompatible with bold/dim */
  u8 fg_index;

  vt_color_t bg_rgb;
  u8 bg_index;

  u8 font_state;
} vt_graphic_rendition_t;

typedef struct vt_char {
  u32 unicode;
  vt_color_t fg;
  vt_color_t bg;
} vt_char_t;

#define VT_BOLD 1
#define VT_DIM 2
#define VT_REVERSE 4

#define TERMCODE(c) "\x07" c
#define TC_RESET "\x01"
#define TC_DIM "\x02"
#define TC_BOLD "\x03"
#define TC_REVERSE "\x04"
#define TC_NO_DIM "\x05"
#define TC_NO_BOLD "\x06"
#define TC_NO_REVERSE "\x07"
#define TC_FG_BLACK "\x11"
#define TC_FG_RED "\x12"
#define TC_FG_GREEN "\x13"
#define TC_FG_YELLOW "\x14"
#define TC_FG_BLUE "\x15"
#define TC_FG_PURPLE "\x16"
#define TC_FG_CYAN "\x17"
#define TC_FG_WHITE "\x18"
#define TC_BG_BLACK "\x21"
#define TC_BG_RED "\x22"
#define TC_BG_GREEN "\x23"
#define TC_BG_YELLOW "\x24"
#define TC_BG_BLUE "\x25"
#define TC_BG_PURPLE "\x26"
#define TC_BG_CYAN "\x27"
#define TC_BG_WHITE "\x28"
void vt_init(u64 drm_number);
void vt_clear();
void vt_flush();
void vt_redraw();
void kprintc(char c);
void kprint(char *str);
void klockv();
void kunlockv();
bool vt_is_available();
