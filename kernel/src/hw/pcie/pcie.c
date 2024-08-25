#include "kernel/hw/pcie/pcie.h"
#include "kernel/debug.h"
#include "kernel/hw/acpi/acpi.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/mm/hhtp.h"
#include <stdbool.h>
#include <stddef.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "pcie"

static u8 region_count;
static PcieRegion regions[256];
static bool initialized = false;

PciHeader *pcie_get_device(u8 b, u8 s, u8 f) {
  PcieRegion *r = NULL;
  for (u8 i = 0; i < region_count; i++) {
    if (regions[i].start <= b && regions[i].end >= b) {
      r = &regions[i];
      break;
    }
  }
  if (!r)
    return NULL;
  u64 rb = b - r->start;
  return (PciHeader *)((rb * 256 + s * 8 + f) * 0x1000 + r->base);
}

bool pcie_init(void (*device_callback)(PciType0 *device)) {
  AcpiMcfg *mcfg = (AcpiMcfg *)acpi_get("MCFG");
  if (!mcfg) {
    log(LOGLEVEL_WARN0, "Couldn't get MCFG");
    return false;
  }
  u64 entries =
      (mcfg->header.length - sizeof(mcfg->header) - sizeof(mcfg->reserved)) /
      sizeof(AcpiMcfgCsbaa);
  if (!entries) {
    log(LOGLEVEL_WARN1, "No PCIe busses found");
    return false;
  }
  logf(LOGLEVEL_DEBUG, "MCFG entries: %u", entries);
  for (u64 i = 0; i < entries; i++) {
    logf(LOGLEVEL_DEBUG, "%u - %u: 0x%l", (u64)mcfg->csbaas[i].start_bus,
         (u64)mcfg->csbaas[i].end_bus, mcfg->csbaas[i].base_addr);
    regions[region_count].start = mcfg->csbaas[i].start_bus;
    regions[region_count].end = mcfg->csbaas[i].end_bus;
    regions[region_count].base = HHDM(mcfg->csbaas[i].base_addr);
    region_count++;
  }
  initialized = true;
  for (u16 b = 0; b < 256; b++) {
    for (u8 s = 0; s < 32; s++) {
      for (u8 f = 0; f < 8; f++) {
        PciHeader *dev = pcie_get_device(b, s, f);
        if (!dev)
          continue;
        if (!dev->vendor_id || dev->vendor_id == 0xffff)
          continue;
        if (!dev->device_id || dev->device_id == 0xffff)
          continue;
#ifdef PCIE_COLOR
        log_lock();
        kprintf(
            "PCIe device at " TERMCODE(TC_DIM) "%u/%u/%u: " TERMCODE(TC_NO_DIM)
                TERMCODE(TC_BOLD) TERMCODE(TC_FG_BLUE) "%w %w " TERMCODE(
                    TC_NO_BOLD) "- " TERMCODE(TC_FG_PURPLE)
                    TERMCODE(TC_BOLD) "%s " TERMCODE(TC_FG_GREEN)
                        TERMCODE(TC_DIM) "%s\n\r" TERMCODE(TC_RESET),
            (u64)b, (u64)s, (u64)f, (int)dev->vendor_id, (int)dev->device_id,
            pci_get_vendor_name(b, s, f), pci_get_device_name(b, s, f));
        log_unlock();
#else
        logf(LOGLEVEL_DEBUG, "PCIe device at %u/%u/%u: %w %w - %s %s", (u64)b,
             (u64)s, (u64)f, (int)dev->vendor_id, (int)dev->device_id,
             pci_get_vendor_name(b, s, f), pci_get_device_name(b, s, f));
#endif
        device_callback((PciType0 *)dev);
      }
    }
  }
  return true;
}

bool pcie_initialized() { return initialized; }
