#pragma once
#include "kernel/gfx/vt/vt.h"

typedef u8 loglevel_t;

#define LOGLEVEL_ANALYZE 0
#define LOGLEVEL_DEBUG 1
#define LOGLEVEL_INFO 2
#define LOGLEVEL_NORMAL 3
#define LOGLEVEL_WARN0 4
#define LOGLEVEL_WARN1 5
#define LOGLEVEL_WARN2 6
#define LOGLEVEL_ERROR 7
#define LOGLEVEL_CRITICAL 8
#define LOGLEVEL_FATAL 9
#define LOGLEVEL_ALWAYS 10

#define str(n) #n
#define xstr(n) str(n)

#ifndef DEBUG_MODULE
#define DEBUG_MODULE "kernel"
#endif

#define debug_module "[" DEBUG_MODULE "] "
#define debug_str debug_module "[" __FILE__ ":" xstr(__LINE__) "] "
#define debug_end TERMCODE(TC_RESET) "\n\r"

#define logf(l, s, f...) _logf(l, debug_str s debug_end, f)
#define log(l, s) _log(l, debug_str s debug_end)

// The debug macros will eventually be removed
#define debug(s) log(LOGLEVEL_DEBUG, s)
#define info(s) log(LOGLEVEL_NORMAL, s)
#define warn(s) log(LOGLEVEL_WARN0, s)
#define error(s) log(LOGLEVEL_CRITICAL, s)

#define debugf(s, f...) logf(LOGLEVEL_DEBUG, s, f)
#define infof(s, f...) logf(LOGLEVEL_INFO, s, f)
#define warnf(s, f...) logf(LOGLEVEL_WARN0, s, f)
#define errorf(s, f...) logf(LOGLEVEL_CRITICAL, s, f)

void set_loglevel(loglevel_t ll);
void set_serial_loglevel(loglevel_t ll);
void _log(loglevel_t ll, char *s);
void _logf(loglevel_t ll, char *s, ...);
