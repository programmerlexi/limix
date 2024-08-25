#pragma once

#include "libk/types.h"
#include <stdbool.h>

#define VMM_ADDRESS(e) (e & (~0xfff))
#define VMM_PRESENT (1 << 0)
#define VMM_WRITEABLE (1 << 1)
#define VMM_PAGE_SIZE (1 << 7)

#define VMM_P_SIZE 0x1000
#define VMM_PT_SIZE 0x200000
#define VMM_PD_SIZE 0x40000000
#define VMM_PDP_SIZE 0x8000000000

#define VMM_PMI2ADDR_INTERNAL(pdpi, pdi, pti, pi)                              \
  (((u64)pdpi << 39) + ((u64)pdi << 30) + ((u64)pti << 21) + ((u64)pi << 12))

#define VMM_PMI2ADDR(pdpi, pdi, pti, pi)                                       \
  mm_fix(VMM_PMI2ADDR_INTERNAL(pdpi, pdi, pti, pi))

#define VMM_FRACTAL_INDEX 260

#define VMM_FRACTAL_PML4                                                       \
  VMM_PMI2ADDR(VMM_FRACTAL_INDEX, VMM_FRACTAL_INDEX, VMM_FRACTAL_INDEX,        \
               VMM_FRACTAL_INDEX)
#define VMM_FRACTAL_PDP(pdp)                                                   \
  VMM_PMI2ADDR(VMM_FRACTAL_INDEX, VMM_FRACTAL_INDEX, VMM_FRACTAL_INDEX, pdp)
#define VMM_FRACTAL_PD(pdp, pd)                                                \
  VMM_PMI2ADDR(VMM_FRACTAL_INDEX, VMM_FRACTAL_INDEX, pdp, pd)
#define VMM_FRACTAL_PT(pdp, pd, pt) VMM_PMI2ADDR(VMM_FRACTAL_INDEX, pdp, pd, pt)

void vmm_init();
uptr vmm_new();

bool vmm_map(void *v, void *p, u64 flags);
void *vmm_mapping(void *v);
