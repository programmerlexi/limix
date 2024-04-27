#include <boot/requests.h>
#include <debug.h>
#include <gfx/font/font.h>
#include <gfx/font/parsers/limefont.h>
#include <gfx/vt/vt.h>
#include <io/serial/serial.h>
#include <kernel.h>
#include <mm/heap.h>
#include <stdint.h>
#include <utils/memory/memory.h>
#include <utils/strings/strings.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "font"

uint8_t *g_8x16_font;

void font_parse() {
  if (module_request.response)
    for (uint64_t i = 0; i < module_request.response->module_count; i++) {
      if (!strncmp(module_request.response->modules[i]->cmdline, "font", 4))
        continue;
      debug("Found font module");
      debug("Attempting limefont parser...");
      if (limefont_parse(module_request.response->modules[i]->address))
        break;
      debug("Limefont failed.");
    }
  if (!g_8x16_font)
    warn("No font loaded using fallback");
  else {
    debug("Font loaded. redrawing!");
    vt_redraw();
  }
}
