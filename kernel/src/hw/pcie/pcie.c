#include "kernel/hw/pcie/pcie.h"
#include "kernel/asm_inline.h"
#include "kernel/debug.h"
#include "kernel/gfx/vt/vt.h"
#include "kernel/hw/acpi/acpi.h"
#include "kernel/hw/cpu/cpu.h"
#include "kernel/hw/pci/codes.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/hw/storage/ahci/ahci.h"
#include "kernel/hw/storage/ide/ide.h"
#include "kernel/hw/storage/nvme/nvme.h"
#include "kernel/hw/usb/xhci/xhci.h"
#include "kernel/mm/hhtp.h"
#include "kernel/task/sched/common.h"
#include "libk/printing.h"
#include <stdbool.h>
#include <stddef.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "pcie"

static u8 region_count;
static pcie_region_t regions[256];
static bool initialized = false;

pci_header_t *pcie_get_device(u8 b, u8 s, u8 f) {
  pcie_region_t *r = NULL;
  for (u8 i = 0; i < region_count; i++) {
    if (regions[i].start <= b && regions[i].end >= b) {
      r = &regions[i];
      break;
    }
  }
  if (!r)
    return NULL;
  u64 rb = b - r->start;
  return (pci_header_t *)((rb * 256 + s * 8 + f) * 0x1000 + r->base);
}

static void pci_handle_device(pci_header_t *dev) {
  switch (dev->class_code) {
  case PCI_CLASS_MASS_STORAGE:
    switch (dev->subclass) {
    case PCI_SUBCLASS_MASS_STORAGE_SATA:
      switch (dev->prog_if) {
      case PCI_PROGIF_MASS_STORAGE_SATA_VENDOR_AHCI:
        ahci_init((pci_type0_t *)dev);
        return;
      }
      return;
    case PCI_SUBCLASS_MASS_STORAGE_NVM:
      nvme_init((pci_type0_t *)dev);
      return;
    case PCI_SUBCLASS_MASS_STORAGE_IDE:
      ide_init((pci_type0_t *)dev);
      return;
    }
    return;
  case PCI_CLASS_BUS:
    switch (dev->subclass) {
    case PCI_SUBCLASS_BUS_USB:
      switch (dev->prog_if) {
      case 0x30:
        xhci_init((pci_type0_t *)dev);
        return;
      }
      return;
    }
    return;
  }
}

bool pcie_init() {
  acpi_mcfg_t *mcfg = (acpi_mcfg_t *)acpi_get("MCFG");
  if (!mcfg) {
    log(LOGLEVEL_WARN0, "Couldn't get MCFG");
    return false;
  }
  u64 entries =
      (mcfg->header.length - sizeof(mcfg->header) - sizeof(mcfg->reserved)) /
      sizeof(acpi_mcfg_csbaa_t);
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
        pci_header_t *dev = pcie_get_device(b, s, f);
        if (!dev)
          continue;
        if (!dev->vendor_id || dev->vendor_id == 0xffff)
          continue;
        if (!dev->device_id || dev->device_id == 0xffff)
          continue;
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
        sched_create(pci_handle_device, cpu_get_random_cpu(), (u64)dev);
      }
    }
  }
  return true;
}

bool pcie_initialized() { return initialized; }
