#include <kernel/debug.h>
#include <kernel/gfx/font/font.h>
#include <kernel/gfx/font/parsers/limefont.h>
#include <kernel/mm/heap.h>
#include <libk/types.h>
#include <libk/utils/memory/memory.h>
#include <libk/utils/strings/strings.h>
#include <stdbool.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "limefont"

bool limefont_parse(void *font) {
  LimefontHeader *head = (LimefontHeader *)font;
  if (!kstrncmp(head->magic, LIMEFONT_MAGIC, 8)) {
    error("Font not loadable: Invalid magic");
    return false;
  }
  g_8x16_font = (u8 *)kmalloc(head->width * head->height * head->glyphs / 8);
  if (!g_8x16_font) {
    error("Font not loadable: Not enough memory");
    return false;
  }
  LimefontRelocation *relocs =
      (LimefontRelocation *)((u64)head + sizeof(LimefontHeader) +
                             head->width * head->height * head->glyphs / 8);
  for (u32 i = 0; i < head->relocs; i++) {
    debug("Copying relocation");
    LimefontRelocation r = relocs[i];
    kmemcpy((void *)((u64)g_8x16_font +
                     (r.target * head->width * head->height / 8)),
            (void *)((u64)head + sizeof(LimefontHeader) +
                     (r.source * head->width * head->height / 8)),
            r.count * (head->width * head->height / 8));
  }
  return true;
}
