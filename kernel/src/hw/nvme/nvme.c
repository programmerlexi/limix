#include "kernel/hw/nvme/nvme.h"
#include "kernel/debug.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/hhtp.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "nvme"

nvme_t *nvme_init(pci_type0_t *h) {
  nvme_t *nvme = kmalloc(sizeof(*nvme));
  nvme->nvme_device = h;
  nvme->bar0 = (void *)HHDM((h->bar0 & PCI_BAR_MEM_BASE_ADDR));
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
