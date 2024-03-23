#pragma once

/* This is essentially just fancy double-buffering */

#include <stdbool.h>
#include <stdint.h>

#define DRM_ATTACHED_TO_SCREEN 1
#define DRM_ATTACHED_TO_PROCESS 2

typedef struct {
  uint32_t *framebuffer;
  uint64_t width;
  uint64_t height;
  uint8_t flags;
} drm_t;

void drm_init();
void drm_switch(uint64_t); // Attach a DRM to the screen
void drm_sync();           // Sync to screen
void drm_plot(uint64_t, uint64_t, uint64_t, uint32_t);
void drm_plot_line(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint32_t);
void drm_plot_rect(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint32_t);
void drm_plot_circle(uint64_t, uint64_t, uint64_t, uint64_t, uint32_t);
void drm_fill_rect(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint32_t);
void drm_clear(uint64_t);
void drm_plot_char(uint64_t, uint64_t, uint64_t, uint32_t, uint32_t);
void drm_fill_rel_rect(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t,
                       uint32_t);
uint64_t drm_width(uint64_t);
uint64_t drm_height(uint64_t);
bool drm_is_attached_to_process(uint64_t);
