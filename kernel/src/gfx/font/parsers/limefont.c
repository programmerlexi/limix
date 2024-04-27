#include <gfx/font/font.h>
#include <gfx/font/parsers/limefont.h>
#include <io/serial/serial.h>
#include <utils/strings/strings.h>

bool limefont_parse(void *font) {
  limefont_header_t *head = (limefont_header_t *)font;
  if (!strncmp(head->magic, LIMEFONT_MAGIC, 8)) {
    serial_writes("Font not loadable: Invalid magic");
    return false;
  }
  g_8x16_font = (uint8_t *)((uint64_t)head +
                            sizeof(limefont_header_t)); // malloc(head->width
                                                        // * head->height *
                                                        // head->glyphs / 8);
  if (!g_8x16_font) {
    serial_writes("Font not loadable: Not enough memory");
    return false;
  }
  return true;
}
