#include "mm/vmm.h"
#include "asm_inline.h"
#include "defines.h"
#include "kernel.h"
#include "mm/hhtp.h"
#include "mm/mm.h"
#include "mm/pmi.h"
#include "utils/memory/memory.h"

static virtual_address_space_t _kas;

void init_kernel_vas() {
  _kas.pml4 = (pml4_t)read_cr4();
  _kas.exists = TRUE;
  _kas.allow_kas_changes = TRUE;
}
virtual_address_space_t init_new_vas() {
  virtual_address_space_t vas;
  vas.pml4 = request_page();
  if (!vas.pml4)
    return vas;
  kmemset(vas.pml4, 0, 0x1000);
  vas.exists = FALSE;
  vas.allow_kas_changes = FALSE;
  vas.pml4[511] = _kas.pml4[511];
  return vas;
}
virtual_address_space_t clone_vas(virtual_address_space_t o) {
  virtual_address_space_t n = init_new_vas();
  for (u64 i = 0; i < 511; i++) {
    if (!(o.pml4[i] & PTE_PRESENT))
      continue;
    pdp_t pdp = (pdp_t)HHDM(PTE_ADDR(o.pml4[i]));
    for (u64 j = 0; j < 512; j++) {
      if (!(pdp[j] & PTE_PRESENT))
        continue;
      pd_t pd = (pd_t)HHDM(PTE_ADDR(pdp[j]));
      for (u64 k = 0; k < 512; k++) {
        if (!(pd[k] & PTE_PRESENT))
          continue;
        pt_t pt = (pt_t)HHDM(PTE_ADDR(pd[k]));
        for (u64 l = 0; l < 512; l++) {
          pte_t pte = pt[l];
          if (!(pte & PTE_PRESENT))
            continue;
          if (!mmap(n,
                    (vaddr_t)((i << (12 + 27)) + (j << (12 + 18)) +
                              (k << (12 + 9)) + (l << 12)),
                    (paddr_t)PTE_ADDR(pte)))
            kernel_panic_error("Failed to clone Address space");
        }
      }
    }
  }
  return n;
}

BOOL kmmap(vaddr_t v, paddr_t p) { return mmap(_kas, v, p); }
BOOL is_kmapped(vaddr_t v) { return is_mapped(_kas, v); }
BOOL kunmap(vaddr_t v) { return munmap(_kas, v); }
paddr_t kmapping(vaddr_t v) { return mapping(_kas, v); }

BOOL mmap(virtual_address_space_t as, vaddr_t v, paddr_t p) {
  if (!as.exists)
    return FALSE;
  pmi_t idx = get_pmi((uintptr_t)v);

  pml4e_t pml4e = as.pml4[idx.pdp_i];
  pdp_t pdp;
  if (!(pml4e & PTE_PRESENT)) {
    pdp = request_page();
    if (!pdp)
      return FALSE;
    kmemset(pdp, 0, 0x1000);
    pml4e = PTE_PRESENT | PTE_WRITABLE | PTE_ADDR(PHY((uintptr_t)pdp));
    as.pml4[idx.pdp_i] = pml4e;
  } else {
    pdp = (pdp_t)HHDM(PTE_ADDR(pml4e));
  }

  pdpe_t pdpe = pdp[idx.pd_i];
  pd_t pd;
  if (!(pdpe & PTE_PRESENT)) {
    pd = request_page();
    if (!pd)
      return FALSE;
    kmemset(pd, 0, 0x1000);
    pdpe = PTE_PRESENT | PTE_WRITABLE | PTE_ADDR(PHY((uintptr_t)pd));
    pdp[idx.pd_i] = pdpe;
  } else {
    pd = (pdp_t)HHDM(PTE_ADDR(pdpe));
  }

  pde_t pde = pd[idx.pt_i];
  pt_t pt;
  if (!(pde & PTE_PRESENT)) {
    pt = request_page();
    if (!pt)
      return FALSE;
    kmemset(pt, 0, 0x1000);
    pde = PTE_PRESENT | PTE_WRITABLE | PTE_ADDR(PHY((uintptr_t)pt));
    pd[idx.pt_i] = pde;
  } else {
    pt = (pdp_t)HHDM(PTE_ADDR(pde));
  }

  pte_t pte = pt[idx.p_i];
  pte = PTE_ADDR((uintptr_t)PHY(p)) | PTE_PRESENT | PTE_WRITABLE;
  pt[idx.p_i] = pte;

  return TRUE;
}
BOOL is_mapped(virtual_address_space_t s, vaddr_t v) {
  return mapping(s, v) != NULL;
}
BOOL munmap(virtual_address_space_t as, vaddr_t v) {
  if (!as.exists)
    return NULL;
  pmi_t idx = get_pmi((uintptr_t)v);

  pml4e_t pml4e = as.pml4[idx.pdp_i];
  pdp_t pdp;
  if (!(pml4e & PTE_PRESENT)) {
    return NULL;
  } else {
    pdp = (pdp_t)PTE_ADDR(pml4e);
  }

  pdpe_t pdpe = pdp[idx.pd_i];
  pd_t pd;
  if (!(pdpe & PTE_PRESENT)) {
    return NULL;
  } else {
    pd = (pdp_t)PTE_ADDR(pdpe);
  }

  pde_t pde = pd[idx.pt_i];
  pt_t pt;
  if (!(pde & PTE_PRESENT)) {
    return NULL;
  } else {
    pt = (pdp_t)PTE_ADDR(pde);
  }

  pte_t pte = pt[idx.p_i];
  pte = 0;
  pt[idx.p_i] = pte;

  return TRUE;
}
paddr_t mapping(virtual_address_space_t as, vaddr_t v) {
  if (!as.exists)
    return NULL;
  uintptr_t o = (uintptr_t)v % 0x1000;
  pmi_t idx = get_pmi((uintptr_t)v);

  pml4e_t pml4e = as.pml4[idx.pdp_i];
  pdp_t pdp;
  if (!(pml4e & PTE_PRESENT)) {
    return NULL;
  } else {
    pdp = (pdp_t)HHDM(PTE_ADDR(pml4e));
  }

  pdpe_t pdpe = pdp[idx.pd_i];
  pd_t pd;
  if (!(pdpe & PTE_PRESENT)) {
    return NULL;
  } else {
    pd = (pdp_t)HHDM(PTE_ADDR(pdpe));
  }

  pde_t pde = pd[idx.pt_i];
  pt_t pt;
  if (!(pde & PTE_PRESENT)) {
    return NULL;
  } else {
    pt = (pdp_t)HHDM(PTE_ADDR(pde));
  }

  pte_t pte = pt[idx.p_i];
  if (!(pte & PTE_PRESENT))
    return NULL;
  return (paddr_t)(PTE_ADDR(pte) + o);
}
