#pragma once
#include <config.h>
#include <gfx/vt/vt.h>
#include <printing.h>

#define str(n) #n
#define xstr(n) str(n)
#ifndef DEBUG_MODULE
#define DEBUG_MODULE "kernel"
#endif
#define debug_module "[" DEBUG_MODULE "] "
#define debug_str debug_module "[" __FILE__ ":" xstr(__LINE__) "] "
#define debug_end TERMCODE(TC_RESET) "\n\r"

#define db_debug TERMCODE(TC_FG_WHITE) TERMCODE(TC_DIM)
#define db_info TERMCODE(TC_FG_WHITE)
#define db_warn TERMCODE(TC_FG_YELLOW)
#define db_error TERMCODE(TC_FG_RED) TERMCODE(TC_BOLD)

#define error(s) kprint(db_error debug_str s debug_end)
#define errorf(s, f...) kprintf(db_error debug_str s debug_end, f)

#if LOGLEVEL > 0
#define warn(s) kprint(db_warn debug_str s debug_end)
#define warnf(s) kprintf(db_warn debug_str s debug_end, f)
#else
#define warn(s) asm("nop")
#define warnf(s, f...) asm("nop")
#endif

#if LOGLEVEL > 1
#define info(s) kprint(db_info debug_str s debug_end)
#define infof(s, f...) kprintf(db_info debug_str s debug_end, f)
#else
#define info(s) asm("nop")
#define infof(s, f...) asm("nop")
#endif

#if LOGLEVEL > 2
#define debug(s) kprint(db_debug debug_str s debug_end)
#define debugf(s, f...) kprintf(db_debug debug_str s debug_end, f)
#else
#define debug(s) asm("nop")
#define debugf(s, f...) asm("nop")
#endif
