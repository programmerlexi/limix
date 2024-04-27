#include <boot/requests.h>
#include <gfx/font/font.h>
#include <kernel.h>
#include <mm/heap.h>
#include <stdint.h>
#include <utils/memory/memory.h>
#include <utils/strings/strings.h>

uint8_t *g_8x16_font;

void font_parse() {
  for (uint64_t i = 0; i < module_request.response->module_count; i++) {
    if (!strncmp(module_request.response->modules[i]->cmdline, "font", 4))
      continue;
    lime_font_header_t *head =
        (lime_font_header_t *)module_request.response->modules[i]->address;
    if (!strncmp(head->magic, LIMEFONT_MAGIC, 8))
      kernel_panic_error("Font not loadable: Invalid magic");
    g_8x16_font = (uint8_t *)((uint64_t)head +
                              sizeof(lime_font_header_t)); // malloc(head->width
                                                           // * head->height *
                                                           // head->glyphs / 8);
    if (!g_8x16_font)
      kernel_panic_error("Font not loadable: Not enough memory");
    /*memcpy(g_8x16_font, (void *)((uint64_t)head + sizeof(lime_font_header_t)),
           head->width * head->height * head->glyphs / 8);*/
    break;
  }
  if (!g_8x16_font)
    kernel_panic_error("Font not found");
}
