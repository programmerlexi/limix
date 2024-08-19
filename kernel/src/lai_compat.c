#include "kernel/debug.h"
#include "kernel/hw/acpi/acpi.h"
#include "kernel/hw/pcie/pcie.h"
#include "kernel/io/pio.h"
#include "kernel/kernel.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/hhtp.h"
#include "lai/drivers/timer.h"
#include "lai/error.h"
#include "lai/host.h"
#include "libk/utils/memory/heap_wrap.h"
#include "libk/utils/strings/strings.h"
#include <stdint.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "lai"

void *laihost_malloc(size_t s) { return kmalloc(s); }
void laihost_free(void *p, size_t s) { return kfree(p); }
void *laihost_realloc(void *p, size_t newsize, size_t oldsize) {
  if (!p)
    return kmalloc(newsize);
  return krealloc(p, newsize, oldsize);
}
void laihost_log(int lai_loglevel, const char *str) {
  switch (lai_loglevel) {
  case LAI_DEBUG_LOG:
    logf(LOGLEVEL_INFO, "%s", str);
    break;
  case LAI_WARN_LOG:
    logf(LOGLEVEL_WARN0, "%s", str);
    break;
  }
}
void laihost_panic(const char *s) { kernel_panic_error(s); }
void *laihost_map(size_t a, size_t s) { return (void *)HHDM(a); }
void laihost_unmap(void *a, size_t s) {}
void *laihost_scan(const char *sig, size_t index) {
  logf(LOGLEVEL_DEBUG, "LAI requested ACPI table: %s", sig);
  if (index > 0)
    kernel_panic_error("Index higher than 0 requested");
  if (kstrncmp("DSDT", (char *)sig, 4)) {
    AcpiFadt *fadt = (AcpiFadt *)acpi_get("FACP");
    if (acpi_is_v2() && fadt->x_dsdt)
      return (void *)HHDM(fadt->x_dsdt);
    return (void *)(u64)HHDM(fadt->dsdt);
  }
  return acpi_get((char *)sig);
}
void laihost_outb(uint16_t port, uint8_t val) { outb(port, val); }
void laihost_outw(uint16_t port, uint16_t val) { outw(port, val); }
void laihost_outd(uint16_t port, uint32_t val) { outd(port, val); }
uint8_t laihost_inb(uint16_t port) { return inb(port); }
uint16_t laihost_inw(uint16_t port) { return inw(port); }
uint32_t laihost_ind(uint16_t port) { return ind(port); }
uint8_t laihost_pci_readb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun,
                          uint16_t offset) {
  u64 a = (u64)pcie_get_device(bus, slot, fun);
  return *(u8 *)(a + offset);
}
uint16_t laihost_pci_readw(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun,
                           uint16_t offset) {
  u64 a = (u64)pcie_get_device(bus, slot, fun);
  return *(u16 *)(a + offset);
}
uint32_t laihost_pci_readd(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun,
                           uint16_t offset) {
  u64 a = (u64)pcie_get_device(bus, slot, fun);
  return *(u32 *)(a + offset);
}
void laihost_pci_writeb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun,
                        uint16_t offset, u8 v) {
  u64 a = (u64)pcie_get_device(bus, slot, fun);
  *(u8 *)(a + offset) = v;
}
void laihost_pci_writew(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun,
                        uint16_t offset, u16 v) {
  u64 a = (u64)pcie_get_device(bus, slot, fun);
  *(u16 *)(a + offset) = v;
}
void laihost_pci_writed(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun,
                        uint16_t offset, u32 v) {
  u64 a = (u64)pcie_get_device(bus, slot, fun);
  *(u32 *)(a + offset) = v;
}
void laihost_sleep(uint64_t t) {
  if (LAI_ERROR_UNSUPPORTED == lai_start_pm_timer()) {
    log(LOGLEVEL_WARN2, "PM timer unsupported");
  }
  lai_busy_wait_pm_timer(t);
}
