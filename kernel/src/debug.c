#include <kernel/config.h>
#include <kernel/debug.h>
#include <kernel/gfx/vt/vt.h>
#include <kernel/io/serial/serial.h>
#include <kernel/mm/heap.h>
#include <libk/ipc/spinlock.h>
#include <libk/printing.h>
#include <stdarg.h>

static loglevel_t _current_loglevel = LOGLEVEL;
static loglevel_t _current_serial_loglevel = LOGLEVEL_SERIAL;

static char *_logcolors[] = {TERMCODE(TC_FG_BLACK) TERMCODE(TC_BOLD),
                             TERMCODE(TC_FG_WHITE) TERMCODE(TC_DIM),
                             TERMCODE(TC_FG_WHITE),
                             TERMCODE(TC_FG_WHITE) TERMCODE(TC_BOLD),
                             TERMCODE(TC_FG_YELLOW) TERMCODE(TC_BOLD),
                             TERMCODE(TC_FG_YELLOW),
                             TERMCODE(TC_FG_YELLOW) TERMCODE(TC_DIM),
                             TERMCODE(TC_FG_RED) TERMCODE(TC_DIM),
                             TERMCODE(TC_FG_RED),
                             TERMCODE(TC_FG_RED) TERMCODE(TC_BOLD),
                             TERMCODE(TC_FG_WHITE) TERMCODE(TC_DIM)};

static u32 lock;

void set_loglevel(loglevel_t ll) { _current_loglevel = ll; }
void set_serial_loglevel(loglevel_t ll) { _current_serial_loglevel = ll; }
void _log(loglevel_t ll, char *s) {
  spinlock(&lock);
  if (ll >= _current_serial_loglevel) {
    serial_writes(s);
  }
  if (ll >= _current_loglevel) {
    kprint(_logcolors[ll]);
    kprint(s);
  }
  spinunlock(&lock);
}
void _logf(loglevel_t ll, char *s, ...) {
  va_list a;
  va_start(a);
  spinlock(&lock);

  char *p = kvfprintf(s, a);

  if (ll >= _current_serial_loglevel) {
    serial_writes(p);
  }

  if (ll >= _current_loglevel) {
    kprint(_logcolors[ll]);
    kprint(p);
  }
  kfree(p);
  spinunlock(&lock);
  va_end(a);
}

void log_lock() { spinlock(&lock); }

void log_unlock() { spinunlock(&lock); }
