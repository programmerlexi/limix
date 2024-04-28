#include <gfx/font/font.h>
#include <gfx/font/parsers/limefont.h>
#include <io/serial/serial.h>
#include <mm/heap.h>
#include <stdint.h>
#include <utils/memory/memory.h>
#include <utils/strings/strings.h>

bool limefont_parse(void *font) {
  limefont_header_t *head = (limefont_header_t *)font;
  if (!strncmp(head->magic, LIMEFONT_MAGIC, 8)) {
    serial_writes("Font not loadable: Invalid magic");
    return false;
  }
  g_8x16_font =
      (uint8_t *)malloc(head->width * head->height * head->glyphs / 8);
  if (!g_8x16_font) {
    serial_writes("Font not loadable: Not enough memory");
    return false;
  }
  limefont_relocation_t *relocs =
      (limefont_relocation_t *)((uint64_t)head + sizeof(limefont_header_t) +
                                head->width * head->height * head->glyphs / 8);
  for (uint32_t i = 0; i < head->relocs; i++) {
    serial_writes("Copying relocation\n\r");
    limefont_relocation_t r = relocs[i];
    memcpy((void *)((uint64_t)g_8x16_font +
                    (r.target * head->width * head->height / 8)),
           (void *)((uint64_t)head + sizeof(limefont_header_t) +
                    (r.source * head->width * head->height / 8)),
           r.count * (head->width * head->height / 8));
  }
  return true;
}
