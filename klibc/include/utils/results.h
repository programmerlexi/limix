#pragma once

#include "utils/variety.h"

enum result_type { Error, Ok };

typedef struct {
  enum result_type t;
  variety_t d;
} result_t;
