#include "utils/memory/heap_wrap.h"
#include "utils/strings/strings.h"
#include "utils/strings/xstr.h"

xstr_t to_xstr(char *cstr) {
  xstr_t xs = {.length = kstrlen(cstr), .cstr = clone(cstr, kstrlen(cstr) + 1)};
  return xs;
}
