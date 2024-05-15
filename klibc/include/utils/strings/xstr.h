#pragma once

#include "types.h"

typedef struct {
  usz length;
  char *cstr;
} xstr_t;

xstr_t to_xstr(char *cstr);
