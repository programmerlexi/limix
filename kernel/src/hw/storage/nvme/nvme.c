#include "kernel/hw/storage/nvme/nvme.h"
#include "kernel/debug.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/hhtp.h"
#include <stddef.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "nvme"

nvme_t *nvme_init(pci_type0_t *h) {
  nvme_t *nvme = kmalloc(sizeof(*nvme));
  nvme->nvme_device = h;
  uptr phys_addr = HHDM((h->bar0 & PCI_BAR_MEM_BASE_ADDR));
  if ((h->bar0 & PCI_BAR_MEM_TYPE >> 1) == 2)
    phys_addr |= (u64)(h->bar1 & PCI_BAR_MEM_BASE_ADDR) << 32;
  nvme->bar0 = (void *)phys_addr;
  logf(LOGLEVEL_ANALYZE, "%l", (u64)nvme->bar0);
  if (!PHY(nvme->bar0)) {
    kfree(nvme);
    log(LOGLEVEL_WARN2, "NVMe driver received nullptr");
    return NULL;
  }
  nvme->admin_submission_queue =
      (void *)HHDM(nvme->bar0->admin_submission_queue);
  nvme->admin_completion_queue =
      (void *)HHDM(nvme->bar0->admin_completion_queue);
  nvme_probe(nvme);
  log(LOGLEVEL_INFO, "Initialized NVMe");
  return nvme;
}

void nvme_probe(nvme_t *nvme) {
  // TODO: Write a working probe function
}
