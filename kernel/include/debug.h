#pragma once
#include <config.h>
#include <gfx/vt/vt.h>

#define str(n) #n
#define xstr(n) str(n)
#define debug_str "[" __FILE__ ":" xstr(__LINE__) "] "
#define debug_end TERMCODE(TC_RESET) "\n\r"

#define db_debug TERMCODE(TC_FG_WHITE) TERMCODE(TC_DIM)
#define db_info TERMCODE(TC_FG_WHITE)
#define db_warn TERMCODE(TC_FG_YELLOW)
#define db_error TERMCODE(TC_FG_RED) TERMCODE(TC_BOLD)

#define error(s) kprint(db_error debug_str s debug_end)
#if LOGLEVEL > 0
#define warn(s) kprint(db_warn debug_str s debug_end)
#if LOGLEVEL > 1
#define info(s) kprint(db_info debug_str s debug_end)
#if LOGLEVEL > 2
#define debug(s) kprint(db_debug debug_str s debug_end)
#else
#define debug(s) asm("nop")
#endif
#else
#define debug(s) asm("nop")
#define info(s) asm("nop")
#endif
#else
#define info(s) asm("nop")
#define warn(s) asm("nop")
#define debug(s) asm("nop")
#endif
