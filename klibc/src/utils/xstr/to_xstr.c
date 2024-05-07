#include <utils/memory/heap_wrap.h>
#include <utils/strings/strings.h>
#include <utils/strings/xstr.h>

xstr_t to_xstr(char *cstr) {
  xstr_t xs = {.length = strlen(cstr), .cstr = clone(cstr, strlen(cstr) + 1)};
  return xs;
}
