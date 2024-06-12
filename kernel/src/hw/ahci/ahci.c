#include "kernel/hw/ahci/ahci.h"
#include "kernel/asm_inline.h"
#include "kernel/debug.h"
#include "kernel/hw/ide/ide.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/io/pio.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/hhtp.h"
#include "kernel/mm/mm.h"
#include "libk/types.h"
#include "libk/utils/memory/memory.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "ahci"

ahci_t *ahci_init(pci_type0_t *h) {
  ahci_t *ahci = kmalloc(sizeof(*ahci));
  ahci->ahci_device = h;
  ahci->abar = (ahci_hba_memory_t *)HHDM(
      (ahci->ahci_device->bar5 & (uptr)PCI_BAR_MEM_BASE_ADDR));
  logf(LOGLEVEL_DEBUG, "Got AHCI ABAR: 0x%l", ahci->abar);
  ahci_probe(ahci);
  for (u32 i = 0; i < ahci->port_count; i++)
    ahci_port_configure(ahci->ports[i]);
  log(LOGLEVEL_INFO, "Initialized AHCI");

  return ahci;
}

void ahci_probe(ahci_t *ahci) {
  u32 ports_implemented = ahci->abar->ports_implemented;
  for (i32 i = 0; i < 32; i++) {
    if (!(ports_implemented & (1 << i)))
      continue;
    ahci_port_type_t type = ahci_check_port_type(&ahci->abar->ports[i]);
    if (type == SATA || type == SATAPI) {
      logf(LOGLEVEL_INFO, "Found SATA/SATAPI device attached at port %u",
           (u64)i);
      ahci->ports[ahci->port_count] = kmalloc(sizeof(ahci_port_t));
      ahci->ports[ahci->port_count]->port_type = type;
      ahci->ports[ahci->port_count]->hba_port = &ahci->abar->ports[i];
      ahci->ports[ahci->port_count]->port_number = i;
      ahci->port_count++;
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

void ahci_port_start_cmd(ahci_port_t *p) {
  log(LOGLEVEL_ANALYZE, "Starting CMD");
  while (p->hba_port->cmd_sts & HBA_PxCMD_CR)
    ;
  p->hba_port->cmd_sts |= HBA_PxCMD_FRE;
  p->hba_port->cmd_sts |= HBA_PxCMD_ST;
  log(LOGLEVEL_ANALYZE, "Started CMD");
}

void ahci_port_stop_cmd(ahci_port_t *p) {
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

void ahci_port_configure(ahci_port_t *p) {
  ahci_port_stop_cmd(p);
  void *new_base = request_page();
  p->hba_port->command_list_base = (u32)PHY(new_base);
  p->hba_port->command_list_base_upper = (u32)(PHY(new_base) >> 32);
  kmemset(new_base, 0, 1024);

  void *fis_base = request_page();
  p->hba_port->fis_base_address = (u32)PHY(fis_base);
  p->hba_port->fis_base_address_upper = (u32)(PHY(fis_base) >> 32);
  kmemset(fis_base, 0, 256);

  ahci_hba_command_header_t *command_header =
      (ahci_hba_command_header_t *)new_base;
  for (i32 i = 0; i < 32; i++) {
    command_header[i].prdt_length = 8;
    void *command_base = request_page();
    u64 addr = PHY(command_base) + (i << 8);
    command_header[i].command_table_base_address = addr;
    command_header[i].command_table_base_address_upper = addr << 32;
    kmemset(command_base, 0, 256);
  }
  ahci_port_start_cmd(p);
  log(LOGLEVEL_INFO, "Configured AHCI port");
}

bool _ahci_port_read_sata(ahci_port_t *p, u64 sector, u32 count, void *buffer) {
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

  ahci_hba_command_header_t *cmd_header = (ahci_hba_command_header_t *)HHDM(
      ((u64)p->hba_port->command_list_base |
       ((u64)p->hba_port->command_list_base_upper << 32)));
  cmd_header->command_fis_length = sizeof(ahci_fis_h2d_t) / sizeof(u32);
  cmd_header->write = 0;
  cmd_header->prdt_length = (u16)((count - 1) >> 4) + 1;

  ahci_hba_command_table_t *cmd_table = (ahci_hba_command_table_t *)HHDM(
      (cmd_header->command_table_base_address |
       ((u64)cmd_header->command_table_base_address_upper << 32)));
  kmemset(cmd_table, 0,
          sizeof(ahci_hba_command_table_t) +
              ((cmd_header->prdt_length - 1) * sizeof(ahci_hba_prdt_entry_t)));

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

  ahci_fis_h2d_t *cmd_fis = (ahci_fis_h2d_t *)HHDM(&cmd_table->command_fis);
  cmd_fis->fis_type = AHCI_FIS_TYPE_REG_H2D;
  cmd_fis->cmd_control = 1;
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

bool ahci_port_read(ahci_port_t *p, u64 sector, u32 count, void *buffer) {
  if (p->port_type == SATA)
    return _ahci_port_read_sata(p, sector, count, buffer);
  else
    return false;
}
