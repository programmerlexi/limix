#pragma once

#include <stdint.h>

typedef struct {
  uint64_t pdp_i;
  uint64_t pd_i;
  uint64_t pt_i;
  uint64_t p_i;
} pmi_t;

pmi_t get_pmi(uint64_t);
