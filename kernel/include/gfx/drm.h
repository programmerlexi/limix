#pragma once

/* This is essentially just fancy double-buffering */

#include <gfx/gfx.h>
#include <stdbool.h>
#include <stdint.h>

#define DRM_ATTACHED_TO_SCREEN 1
#define DRM_ATTACHED_TO_PROCESS 2

typedef struct {
  uint32_t *framebuffer;
  uint64_t width;
  uint64_t height;
  uint8_t flags;
  uint32_t lock;
} drm_t;

typedef uint64_t drm_number_t;

void drm_init();
void drm_switch(drm_number_t drm_number); // Attach a DRM to the screen
void drm_sync();                          // Sync to screen
void drm_plot(drm_number_t drm_number, absolute64_t x, absolute64_t y,
              color32_t color);
void drm_plot_line(drm_number_t drm_number, absolute64_t x0, absolute64_t y0,
                   absolute64_t x1, absolute64_t y1, color32_t color);
void drm_plot_rect(drm_number_t drm_number, absolute64_t x0, absolute64_t y0,
                   absolute64_t x1, absolute64_t y1, color32_t color);
void drm_plot_circle(drm_number_t drm_number, absolute64_t centerx,
                     absolute64_t centery, offset64_t radius, color32_t color);
void drm_fill_rect(drm_number_t drm_number, absolute64_t x0, absolute64_t y0,
                   absolute64_t x1, absolute64_t y1, color32_t color);
void drm_clear(drm_number_t drm_number);
void drm_plot_char(drm_number_t drm_number, absolute64_t x, absolute64_t y,
                   index32_t character_index, color32_t color);
void drm_plot_char_solid(drm_number_t drm_number, absolute64_t x,
                         absolute64_t y, index32_t character_index,
                         color32_t color, color32_t bg);
void drm_fill_rel_rect(drm_number_t drm_number, absolute64_t x, absolute64_t y,
                       offset64_t w, offset64_t h, color32_t color);
absolute64_t drm_width(drm_number_t drm_number);
absolute64_t drm_height(drm_number_t drm_number);
bool drm_is_attached_to_process(drm_number_t drm_number);
