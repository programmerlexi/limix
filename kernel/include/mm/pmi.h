#pragma once

#include "types.h"

typedef struct {
  u64 pdp_i;
  u64 pd_i;
  u64 pt_i;
  u64 p_i;
} pmi_t;

pmi_t get_pmi(u64 addr);
