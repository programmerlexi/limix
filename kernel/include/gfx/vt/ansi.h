#pragma once

#include <gfx/vt/vt.h>
#include <stdbool.h>

enum ANSI_STATE { C1, CSI_CMD, ARG, SEMICOLON };

typedef struct ansi_state {
  char args[8];
  bool working;
  enum ANSI_STATE as;
  vt_graphic_rendition_t gr;
} ansi_state_t;

/* C0 codes */
#define BEL '\x7'
#define BS '\x8'
#define HT '\x9'
#define LF '\xa'
#define CR '\xd'
#define ESC '\x1b'
#define ESC_SEQ "\x1b"

/* C1 Sequences */
#define SS2 ESC_SEQ "N"
#define SS3 ESC_SEQ "O"
#define CSI ESC_SEQ "["

/* CSI Command chars */
#define CUU(n) CSI n "A"
#define CUD(n) CSI n "B"
#define CUF(n) CSI n "C"
#define CUB(n) CSI n "D"
#define CNL(n) CSI n "E"
#define CPL(n) CSI n "F"
#define CHA(n) CSI n "G"
#define CUP(x, y) CSI x ";" y "H"
#define ED(n) CSI n "J"
#define EL(n) CSI n "K"
#define SU(n) CSI n "S"
#define SD(n) CSI n "T"
#define HVP(n, m) CSI n ";" m "f"
#define SGR(gr) CSI gr "m"

/* Graphic Rendition */
#define SGR_RESET "0"
#define SGR_BOLD "1"
#define SGR_DIM "2"
#define SGR_UNDERLINE "4"
#define SGR_INVERT "7"
#define SGR_STRIKE "9"
#define SGR_DOUBLE_UNDERLINE "21"
#define SGR_NORMAL "22"
#define SGR_NO_UNDERLINE "24"
#define SGR_NO_REVERSE "27"
#define SGR_NO_STRIKE "29"

#define SGR_FORE_BLACK "30"
#define SGR_FORE_RED "31"
#define SGR_FORE_GREEN "32"
#define SGR_FORE_YELLOW "33"
#define SGR_FORE_BLUE "34"
#define SGR_FORE_MAGENTA "35"
#define SGR_FORE_CYAN "36"
#define SGR_FORE_WHITE "37"

#define SGR_FORE_CI(ci) "38;5;" ci
#define SGR_FORE_RGB(r, g, b) "38;2;" r ";" g ";" b

#define SGR_FORE_DEFAULT "39"

#define SGR_BACK_BLACK "30"
#define SGR_BACK_RED "31"
#define SGR_BACK_GREEN "32"
#define SGR_BACK_YELLOW "33"
#define SGR_BACK_BLUE "34"
#define SGR_BACK_MAGENTA "35"
#define SGR_BACK_CYAN "36"
#define SGR_BACK_WHITE "37"

#define SGR_BACK_CI(ci) "48;5;" ci
#define SGR_BACK_RGB(r, g, b) "48;2;" r ";" g ";" b

#define SGR_BACK_DEFAULT "49"

#define SGR_OVERLINE "53"
#define SGR_NO_OVERLINE "55"

#define SGR_UNDERLINE_COLOR_CI(ci) "58;5;" ci
#define SGR_UNDERLINE_COLOR_RGB(r, g, b) "58;2;" r ";" g ";" b

#define SGR_DEFAULT_UNDERLINE_COLOR "59"

ansi_state_t ansi_process(ansi_state_t, char);
uint32_t ansi_convert_fg(vt_graphic_rendition_t);
uint32_t ansi_convert_bg(vt_graphic_rendition_t);
