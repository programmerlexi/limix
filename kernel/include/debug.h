#pragma once
#include <gfx/vt/vt.h>

#define str(n) #n
#define xstr(n) str(n)
#define debug_str "[" __FILE__ ":" xstr(__LINE__) "] "
#define debug_end TERMCODE(TC_RESET) "\n\r"

#define db_debug TERMCODE(TC_FG_WHITE) TERMCODE(TC_DIM)
#define db_info TERMCODE(TC_FG_WHITE)
#define db_warn TERMCODE(TC_FG_YELLOW)
#define db_error TERMCODE(TC_FG_RED) TERMCODE(TC_BOLD)

#define debug(s) kprint(db_debug debug_str s debug_end)
#define info(s) kprint(db_info debug_str s debug_end)
#define warn(s) kprint(db_warn debug_str s debug_end)
#define error(s) kprint(db_error debug_str s debug_end)
