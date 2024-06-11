#include "kernel/hw/nvme/nvme.h"
#include "kernel/debug.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/hhtp.h"

nvme_t *nvme_init(pci_type0_t *h) {
  nvme_t *nvme = kmalloc(sizeof(*nvme));
  nvme->nvme_device = h;
  nvme->bar0 = (void *)HHDM(h->bar0);
  log(LOGLEVEL_INFO, "Initialized NVMe");
  return nvme;
}
