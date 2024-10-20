#include <kernel/asm_inline.h>
#include <kernel/constructors.h>
#include <kernel/debug.h>
#include <kernel/hw/devman/devman.h>
#include <kernel/hw/pci/codes.h>
#include <kernel/hw/pci/pci.h>
#include <kernel/hw/storage/ahci/ahci.h>
#include <kernel/hw/storage/ide/ide.h>
#include <kernel/initgraph.h>
#include <kernel/io/pio.h>
#include <kernel/kernel.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/hhtp.h>
#include <kernel/mm/pmm.h>
#include <libk/types.h>
#include <libk/utils/memory/memory.h>
#include <libk/utils/memory/safety.h>
#include <stdbool.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "ahci"

static void dm_register_ahci() {
  devman_register_driver(CLASSSUBCLASSPROGIF, PCI_CLASS_MASS_STORAGE,
                         PCI_SUBCLASS_MASS_STORAGE_SATA,
                         PCI_PROGIF_MASS_STORAGE_SATA_VENDOR_AHCI, ahci_init);
}

CONSTRUCTOR(ahci) {
  INITGRAPH_NODE("ahci_driver", dm_register_ahci);
  INITGRAPH_NODE_DEP("ahci_driver", "devman");
  INITGRAPH_NODE_STAGE("ahci_driver", "drivers");
}

bool ahci_init(PciType0 *h) {
  Ahci *ahci = kmalloc(sizeof(*ahci));
  ahci->ahci_device = h;
  ahci->abar = (AhciHbaMemory *)HHDM(
      (ahci->ahci_device->bar5 & (uptr)PCI_BAR_MEM_BASE_ADDR));
  ahci->port_count = 0;
  logf(LOGLEVEL_DEBUG, "Got AHCI ABAR: 0x%l", ahci->abar);
  ahci_probe(ahci);
  for (u32 i = 0; i < ahci->port_count; i++) {
    ahci_port_configure(ahci->ports[i]);
    DevmanStorageType st;
    if (ahci->ports[i]->port_type == SATA)
      st = HDD;
    else if (ahci->ports[i]->port_type == SATAPI)
      st = CD;
    else
      kernel_panic_error("Logic error");
    devman_add_storage(st, ahci->ports[i],
                       (bool (*)(void *, u64, u32, void *))ahci_port_read,
                       (bool (*)(void *, u64, u32, void *))ahci_port_write,
                       (bool (*)(void *))ahci_port_check);
  }
  log(LOGLEVEL_INFO, "Initialized AHCI");

  return true;
}

void ahci_probe(Ahci *ahci) {
  u32 ports_implemented = ahci->abar->ports_implemented;
  for (i32 i = 0; i < 32; i++) {
    if (!(ports_implemented & (1 << i)))
      continue;
    AhciPortType type = ahci_check_port_type(&ahci->abar->ports[i]);
    if (type == SATA) {
      logf(LOGLEVEL_INFO, "Found SATA device attached at port %u", (u64)i);
      ahci->ports[ahci->port_count] = kmalloc(sizeof(AhciPort));
      invlpg(ahci->ports[ahci->port_count]);
      nullsafe_error(ahci->ports[ahci->port_count], "Out of memory");
      ahci->ports[ahci->port_count]->port_type = type;
      ahci->ports[ahci->port_count]->hba_port = &ahci->abar->ports[i];
      ahci->ports[ahci->port_count]->port_number = i;
      ahci->port_count++;
    }
  }
}
AhciPortType ahci_check_port_type(AhciHbaPort *port) {
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

void ahci_port_start_cmd(AhciPort *p) {
  log(LOGLEVEL_ANALYZE, "Starting CMD");
  while (p->hba_port->cmd_sts & HBA_PxCMD_CR)
    ;
  p->hba_port->cmd_sts |= HBA_PxCMD_FRE;
  p->hba_port->cmd_sts |= HBA_PxCMD_ST;
  log(LOGLEVEL_ANALYZE, "Started CMD");
}

void ahci_port_stop_cmd(AhciPort *p) {
  log(LOGLEVEL_ANALYZE, "Stopping CMD");
  p->hba_port->cmd_sts &= ~HBA_PxCMD_ST;
  p->hba_port->cmd_sts &= ~HBA_PxCMD_FRE;

  io_wait();

  while (true) {
    if (p->hba_port->cmd_sts & HBA_PxCMD_FR)
      continue;
    if (p->hba_port->cmd_sts & HBA_PxCMD_CR)
      continue;

    break;
  }
  log(LOGLEVEL_ANALYZE, "Stopped CMD");
}

void ahci_port_configure(AhciPort *p) {
  ahci_port_stop_cmd(p);
  void *new_base = (void *)HHDM(request_dma(1));
  p->hba_port->command_list_base = (u32)PHY(new_base);
  p->hba_port->command_list_base_upper = (u32)(PHY(new_base) >> 32);
  kmemset(new_base, 0, 1024);

  void *fis_base = (void *)HHDM(request_dma(1));
  p->hba_port->fis_base_address = (u32)PHY(fis_base);
  p->hba_port->fis_base_address_upper = (u32)(PHY(fis_base) >> 32);
  kmemset(fis_base, 0, 256);

  AhciCommandHeader *command_header = (AhciCommandHeader *)new_base;
  for (i32 i = 0; i < 32; i++) {
    command_header[i].prdt_length = 8;
    void *command_base = (void *)HHDM(request_dma(1));
    u64 addr = PHY(command_base) + (i << 8);
    command_header[i].command_table_base_address = addr;
    command_header[i].command_table_base_address_upper = addr << 32;
    kmemset(command_base, 0, 256);
  }
  ahci_port_start_cmd(p);
  log(LOGLEVEL_INFO, "Configured AHCI port");
}

bool _ahci_port_io_sata(AhciPort *p, u64 sector, u32 count, void *buffer,
                        bool write) {
  invlpg(buffer);

  p->hba_port->interrupt_status = (u32)-1;

  u64 spin = 0;
  while ((p->hba_port->task_file_data & (ATA_SR_BSY | ATA_SR_DRQ)) &&
         spin < 1000000) {
    io_wait();
    spin++;
  }
  if (spin == 1000000)
    return false;

  AhciCommandHeader *cmd_header = (AhciCommandHeader *)HHDM(
      ((u64)p->hba_port->command_list_base |
       ((u64)p->hba_port->command_list_base_upper << 32)));
  cmd_header->command_fis_length = sizeof(AhciFisH2D) / sizeof(u32);
  cmd_header->write = write;
  cmd_header->prdt_length = (u16)((count - 1) >> 4) + 1;

  AhciHbaCommandTable *cmd_table = (AhciHbaCommandTable *)HHDM(
      (cmd_header->command_table_base_address |
       ((u64)cmd_header->command_table_base_address_upper << 32)));
  kmemset(cmd_table, 0,
          sizeof(AhciHbaCommandTable) +
              ((cmd_header->prdt_length - 1) * sizeof(AhciHbaPrdtEntry)));

  u64 buf = PHY(buffer);
  i32 i;
  for (i = 0; i < cmd_header->prdt_length - 1; i++) {
    cmd_table->prdt_entry[i].data_base[0] = (u32)buf;
    cmd_table->prdt_entry[i].data_base[1] = (u32)(buf >> 32);
    cmd_table->prdt_entry[i].byte_count = 0x2000 - 1;
    cmd_table->prdt_entry[i].interrupt_on_completion = 1;
    buf += 0x2000;
    count -= 16;
  }
  cmd_table->prdt_entry[i].data_base[0] = (u32)buf;
  cmd_table->prdt_entry[i].data_base[1] = (u32)(buf >> 32);
  cmd_table->prdt_entry[i].byte_count = (count << 9) - 1;
  cmd_table->prdt_entry[i].interrupt_on_completion = 1;

  AhciFisH2D *cmd_fis = (AhciFisH2D *)HHDM(&cmd_table->command_fis);
  cmd_fis->fis_type = AHCI_FIS_TYPE_REG_H2D;
  cmd_fis->cmd_control = 1;
  if (write)
    cmd_fis->cmd = ATA_CMD_WRITE_DMA_EXT;
  else
    cmd_fis->cmd = ATA_CMD_READ_DMA_EXT;
  cmd_fis->lba_low[0] = (u8)sector;
  cmd_fis->lba_low[1] = (u8)(sector >> 8);
  cmd_fis->lba_low[2] = (u8)(sector >> 16);
  cmd_fis->lba_high[0] = (u8)(sector >> 24);
  cmd_fis->lba_high[1] = (u8)(sector >> 32);
  cmd_fis->lba_high[2] = (u8)(sector >> 40);
  cmd_fis->device_register = 1 << 6; // LBA mode
  cmd_fis->count[0] = count & 0xff;
  cmd_fis->count[1] = (count >> 8) & 0xff;
  p->hba_port->command_issue = 1;
  spin = 0;
  while (spin < 500000) {
    io_wait();
    if ((p->hba_port->command_issue & 1) == 0)
      break;
    if (p->hba_port->interrupt_status & HBA_PxIS_TFES)
      return false;
    spin++;
  }
  if (spin == 500000) {
    log(LOGLEVEL_ERROR, "AHCI seems to be quite slow today");
    return false;
  }
  if (p->hba_port->interrupt_status & HBA_PxIS_TFES)
    return false;
  return true;
}

bool ahci_port_read(AhciPort *p, u64 sector, u32 count, void *buffer) {
  if (p->port_type == SATA)
    return _ahci_port_io_sata(p, sector, count, buffer, false);
  else
    return false;
}

bool ahci_port_write(AhciPort *p, u64 sector, u32 count, void *buffer) {
  if (p->port_type == SATA)
    return _ahci_port_io_sata(p, sector, count, buffer, true);
  else
    return false;
}

bool ahci_port_check(AhciPort *p) {
  return true; // TODO: Check
}
