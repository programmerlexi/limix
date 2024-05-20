#include "kernel/gfx/font/parsers/limefont.h"
#include "kernel/debug.h"
#include "kernel/gfx/font/font.h"
#include "kernel/mm/heap.h"
#include "libk/types.h"
#include "libk/utils/memory/memory.h"
#include "libk/utils/strings/strings.h"
#include <stdbool.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "limefont"

bool limefont_parse(void *font) {
  limefont_header_t *head = (limefont_header_t *)font;
  if (!kstrncmp(head->magic, LIMEFONT_MAGIC, 8)) {
    error("Font not loadable: Invalid magic");
    return false;
  }
  g_8x16_font = (u8 *)kmalloc(head->width * head->height * head->glyphs / 8);
  if (!g_8x16_font) {
    error("Font not loadable: Not enough memory");
    return false;
  }
  limefont_relocation_t *relocs =
      (limefont_relocation_t *)((u64)head + sizeof(limefont_header_t) +
                                head->width * head->height * head->glyphs / 8);
  for (u32 i = 0; i < head->relocs; i++) {
    debug("Copying relocation");
    limefont_relocation_t r = relocs[i];
    kmemcpy((void *)((u64)g_8x16_font +
                     (r.target * head->width * head->height / 8)),
            (void *)((u64)head + sizeof(limefont_header_t) +
                     (r.source * head->width * head->height / 8)),
            r.count * (head->width * head->height / 8));
  }
  return true;
}
