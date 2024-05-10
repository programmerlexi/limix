#include <config.h>
#include <debug.h>
#include <gfx/vt/vt.h>
#include <stdarg.h>

static loglevel_t _current_loglevel = LOGLEVEL;

static char *_logcolors[] = {TERMCODE(TC_FG_BLACK) TERMCODE(TC_BOLD),
                             TERMCODE(TC_FG_WHITE) TERMCODE(TC_DIM),
                             TERMCODE(TC_FG_WHITE),
                             TERMCODE(TC_FG_WHITE) TERMCODE(TC_BOLD),
                             TERMCODE(TC_FG_YELLOW) TERMCODE(TC_BOLD),
                             TERMCODE(TC_FG_YELLOW),
                             TERMCODE(TC_FG_YELLOW) TERMCODE(TC_DIM),
                             TERMCODE(TC_FG_RED) TERMCODE(TC_DIM),
                             TERMCODE(TC_FG_RED),
                             TERMCODE(TC_FG_RED) TERMCODE(TC_BOLD)};

void set_loglevel(loglevel_t ll) { _current_loglevel = ll; }
void _log(loglevel_t ll, char *s) {
  if (ll >= _current_loglevel) {
    kprint(_logcolors[ll]);
    kprint(s);
  }
}
void _logf(loglevel_t ll, char *s, ...) {
  if (ll >= _current_loglevel) {
    kprint(_logcolors[ll]);
    va_list a;
    va_start(a, s);
    kvprintf(s, a);
    va_end(a);
  }
}
