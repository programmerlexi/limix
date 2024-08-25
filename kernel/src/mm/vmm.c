#include "kernel/mm/vmm.h"
#include "kernel/asm_inline.h"
#include "kernel/kernel.h"
#include "kernel/mm/hhtp.h"
#include "kernel/mm/mm.h"
#include "kernel/mm/pmm.h"
#include "libk/types.h"
#include "libk/utils/memory/memory.h"
#include <stddef.h>

void vmm_init() {
  u64 *pml4 = (u64 *)HHDM(read_cr3());
  pml4[VMM_FRACTAL_INDEX] =
      VMM_ADDRESS(PHY(pml4)) | VMM_PRESENT | VMM_WRITEABLE;
  mark_pages((void *)read_cr3(), 1);
  write_cr3(read_cr3());
}
uptr vmm_new() {
  u64 *pml4 = (u64 *)HHDM(request_page());
  u64 *op = (u64 *)HHDM((read_cr3() & ~0xfff));
  kmemcpy(pml4, op, 0x1000);
  kmemset(pml4, 0, 0x800);
  pml4[VMM_FRACTAL_INDEX] =
      VMM_ADDRESS(PHY(pml4)) | VMM_PRESENT | VMM_WRITEABLE;
  return (uptr)PHY(pml4);
}

bool vmm_map(void *v, void *p, u64 flags) {
  u64 pdp_i = (((uptr)v) >> 39) & 0x1ff;
  u64 pd_i = (((uptr)v) >> 30) & 0x1ff;
  u64 pt_i = (((uptr)v) >> 21) & 0x1ff;
  u64 p_i = (((uptr)v) >> 12) & 0x1ff;

  u64 *pml4 = (u64 *)VMM_FRACTAL_PML4;
  u64 pdpe = pml4[pdp_i];
  if (!(pdpe & VMM_PRESENT)) {
    pml4[pdp_i] =
        VMM_ADDRESS((uptr)request_page()) | VMM_PRESENT | VMM_WRITEABLE;
    kmemset((void *)HHDM(VMM_ADDRESS(pml4[pdp_i])), 0, 0x1000);
  }
  if (pdpe & VMM_PAGE_SIZE)
    kernel_panic_error("PAGING: PS bit not supported");

  u64 *pdp = (u64 *)VMM_FRACTAL_PDP(pdp_i);
  u64 pde = pdp[pd_i];
  if (!(pde & VMM_PRESENT)) {
    pdp[pd_i] = VMM_ADDRESS((uptr)request_page()) | VMM_PRESENT | VMM_WRITEABLE;
    kmemset((void *)HHDM(VMM_ADDRESS(pdp[pd_i])), 0, 0x1000);
  }
  if (pde & VMM_PAGE_SIZE)
    kernel_panic_error("PAGING: PS bit not supported");

  u64 *pd = (u64 *)VMM_FRACTAL_PD(pdp_i, pd_i);
  u64 pte = pd[pt_i];
  if (!(pte & VMM_PRESENT)) {
    pd[pt_i] = VMM_ADDRESS((uptr)request_page()) | VMM_PRESENT | VMM_WRITEABLE;
    kmemset((void *)HHDM(VMM_ADDRESS(pd[pt_i])), 0, 0x1000);
  }
  if (pte & VMM_PAGE_SIZE)
    kernel_panic_error("PAGING: PS bit not supported");

  u64 *pt = (u64 *)VMM_FRACTAL_PT(pdp_i, pd_i, pt_i);
  pt[p_i] = VMM_ADDRESS((uptr)p) | VMM_PRESENT | flags;
  write_cr3(read_cr3());
  return true;
}
void *vmm_mapping(void *v) {
  u64 pdp_i = (((uptr)v) >> 39) & 0x1ff;
  u64 pd_i = (((uptr)v) >> 30) & 0x1ff;
  u64 pt_i = (((uptr)v) >> 21) & 0x1ff;
  u64 p_i = (((uptr)v) >> 12) & 0x1ff;

  u64 *pml4 = (u64 *)VMM_FRACTAL_PML4;
  u64 pdpe = pml4[pdp_i];
  if (!(pdpe & VMM_PRESENT))
    return NULL;
  if (pdpe & VMM_PAGE_SIZE)
    kernel_panic_error("PAGING: PS bit not supported");

  u64 *pdp = (u64 *)VMM_FRACTAL_PDP(pdp_i);
  u64 pde = pdp[pd_i];
  if (!(pde & VMM_PRESENT))
    return NULL;
  if (pde & VMM_PAGE_SIZE)
    kernel_panic_error("PAGING: PS bit not supported");

  u64 *pd = (u64 *)VMM_FRACTAL_PD(pdp_i, pd_i);
  u64 pte = pd[pt_i];
  if (!(pte & VMM_PRESENT))
    return NULL;
  if (pte & VMM_PAGE_SIZE)
    kernel_panic_error("PAGING: PS bit not supported");

  u64 *pt = (u64 *)VMM_FRACTAL_PT(pdp_i, pd_i, pt_i);
  if (pt[p_i] & VMM_PRESENT)
    return (void *)VMM_ADDRESS(pt[p_i]);
  return NULL;
}
