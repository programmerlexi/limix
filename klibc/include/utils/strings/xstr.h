#pragma once

#include <stddef.h>

typedef struct {
  size_t length;
  char *cstr;
} xstr_t;

xstr_t to_xstr(char *cstr);
