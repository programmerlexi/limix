#include "mm/pmi.h"

pmi_t get_pmi(u64 a) {
  pmi_t pmi = {.p_i = (a >> (12 + 0)) & 0x1ff,
               .pt_i = (a >> (12 + 9)) & 0x1ff,
               .pd_i = (a >> (12 + 18)) & 0x1ff,
               .pdp_i = (a >> (12 + 27)) & 0x1ff};
  return pmi;
}
