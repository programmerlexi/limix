#include "kernel/hw/ahci/ahci.h"
#include "kernel/debug.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/hw/pcie/pcie.h"
#include "kernel/kernel.h"
#include "kernel/mm/hhtp.h"
#include "libk/types.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "ahci"

static ahci_hba_memory_t *abar;
static pci_type0_t *ahci_device;

void ahci_init() {
  if (!pcie_initialized())
    kernel_panic_error("No PCIe present");
  u64 count = pci_get_count(0x8086, 0x2922);
  if (!count)
    kernel_panic_error("No supported AHCI devices present");
  pci_address_t ahci_address = pci_search(0x8086, 0x2922, 0);
  ahci_device = (pci_type0_t *)pcie_get_device(
      ahci_address.bus, ahci_address.slot, ahci_address.func);
  abar = (ahci_hba_memory_t *)HHDM(ahci_device->bar5);
  logf(LOGLEVEL_DEBUG, "Got AHCI ABAR: 0x%l", abar);
  ahci_probe();
}

void ahci_probe() {
  u32 ports_implemented = abar->ports_implemented;
  for (i32 i = 0; i < 32; i++) {
    if (!(ports_implemented & (1 << i)))
      continue;
    ahci_port_type_t type = ahci_check_port_type(&abar->ports[i]);
    switch (type) {
    case SATA:
      logf(LOGLEVEL_INFO, "Port %u: SATA", i);
      break;
    case SATAPI:
      logf(LOGLEVEL_INFO, "Port %u: SATAPI", i);
      break;
    default:
      break;
    }
  }
}
ahci_port_type_t ahci_check_port_type(ahci_hba_port_t *port) {
  u32 sata_status = port->sata_status;

  u8 interface_power_management = (sata_status >> 8) & 0b111;
  u8 device_detection = sata_status & 0b111;

  if (device_detection != HBA_PORT_DEV_PRESENT)
    return NONE;
  if (interface_power_management != HBA_PORT_IPM_ACTIVE)
    return NONE;

  switch (port->signature) {
  case SATA_SIG_ATAPI:
    return SATAPI;
  case SATA_SIG_ATA:
    return SATA;
  case SATA_SIG_PM:
    return PM;
  case SATA_SIG_SEMB:
    return SEMB;
  default:
    return NONE;
  }
}
