#include "gfx/font/font.h"
#include "boot/requests.h"
#include "debug.h"
#include "gfx/font/parsers/limefont.h"
#include "gfx/vt/vt.h"
#include "utils/strings/strings.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "font"

u8 *g_8x16_font;

void font_parse() {
  if (g_module_request.response)
    for (u64 i = 0; i < g_module_request.response->module_count; i++) {
      if (!kstrncmp(g_module_request.response->modules[i]->cmdline, "font", 4))
        continue;
      debug("Found font module");
      debug("Attempting limefont parser...");
      if (limefont_parse(g_module_request.response->modules[i]->address))
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
