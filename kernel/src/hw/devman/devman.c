#include "kernel/hw/devman/devman.h"
#include "kernel/debug.h"
#include "kernel/fs/gpt.h"
#include "kernel/hw/pci/codes.h"
#include "kernel/hw/pcie/pcie.h"
#include "kernel/hw/storage/ahci/ahci.h"
#include "kernel/kernel.h"
#include "kernel/mm/heap.h"
#include "libk/ipc/spinlock.h"
#include "libk/utils/memory/heap_wrap.h"
#include <stddef.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "devman"

static DevmanStorage **storage_pointers;
static PcieDriverHandle *pcie_drivers = NULL;
static u64 storages;
static u64 highest_storage;
static u32 storage_lock = 1;
static u32 driver_lock;

static void devman_handle_pcie_device(PciType0 *device) {
  spinlock(&driver_lock);
  PcieDriverHandle *d = pcie_drivers;
  while (d) {
    switch (d->address_type) {
    case VENDOR:
      if (d->address[0] == device->h.vendor_id)
        if (d->init(device))
          goto end;
      break;
    case VENDORDEVICE:
      if (d->address[0] == device->h.vendor_id &&
          d->address[1] == device->h.device_id)
        if (d->init(device))
          goto end;
      break;
    case CLASS:
      if (d->address[0] == device->h.class_code)
        if (d->init(device))
          goto end;
      break;
    case CLASSSUBCLASS:
      if (d->address[0] == device->h.class_code &&
          d->address[1] == device->h.subclass)
        if (d->init(device))
          goto end;
      break;
    case CLASSSUBCLASSPROGIF:
      if (d->address[0] == device->h.class_code &&
          d->address[1] == device->h.subclass &&
          d->address[2] == device->h.prog_if)
        if (d->init(device))
          goto end;
      break;
    }
    d = d->next;
  }
  logf(LOGLEVEL_WARN0, "No driver for device %w:%w (%w.%w.%w)",
       device->h.vendor_id, device->h.device_id, device->h.class_code,
       device->h.subclass, device->h.prog_if);
end:
  spinunlock(&driver_lock);
}

void devman_init() {
  log(LOGLEVEL_INFO, "Initializing...");
  storage_pointers = NULL;
  storages = 0;
  highest_storage = 0;
  spinunlock(&storage_lock);
  log(LOGLEVEL_INFO, "Initialized...");
}

void devman_add_drivers() {
  log(LOGLEVEL_INFO, "Adding drivers to driver list...");
  devman_register_driver(CLASSSUBCLASSPROGIF, PCI_CLASS_MASS_STORAGE,
                         PCI_SUBCLASS_MASS_STORAGE_SATA,
                         PCI_PROGIF_MASS_STORAGE_SATA_VENDOR_AHCI, ahci_init);
}

void devman_enumerate() {
  log(LOGLEVEL_INFO, "Enumerating devices...");
  if (!pcie_init(devman_handle_pcie_device))
    kernel_panic_error("PCIe init failed");
}

void devman_register_driver(enum PcieDriverAddressTypeEnum address_type,
                            u16 addr0, u16 addr1, u16 addr2,
                            bool (*init)(PciType0 *device)) {
  PcieDriverHandle *driver_handle = kmalloc(sizeof(*driver_handle));
  driver_handle->init = init;
  driver_handle->address[0] = addr0;
  driver_handle->address[1] = addr1;
  driver_handle->address[2] = addr2;
  driver_handle->address_type = address_type;
  spinlock(&driver_lock);
  driver_handle->next = pcie_drivers;
  pcie_drivers = driver_handle;
  spinunlock(&driver_lock);
}

static DevmanStoragePartition _devman_get_partition(u64 drive, u64 partition) {
  DevmanStoragePartition r;
  r.id = 0;
  r.start = 0;
  r.end = 0;
  if (!partition) {
    r.id = 0;
    r.start = 0;
    r.end = -1;
  } else {
    if (!(drive < highest_storage))
      return r;
    for (u64 i = 0; i < storages; i++) {
      if (!storage_pointers[i])
        continue;
      if (storage_pointers[i]->number == drive) {
        if (!storage_pointers[i]->partitions)
          break;
        for (u64 j = 0; j < storage_pointers[i]->partition_count; j++) {
          if (!storage_pointers[i]->partitions[j])
            continue;
          if (storage_pointers[i]->partitions[j]->id == partition) {
            r.start = storage_pointers[i]->partitions[j]->start;
            r.end = storage_pointers[i]->partitions[j]->end;
            r.id = storage_pointers[i]->partitions[j]->id;
            break;
          }
        }
        break;
      }
    }
  }
  return r;
}

static DevmanStorage *_devman_get_storage(u64 drive) {
  if (!storage_pointers)
    return NULL;
  for (u64 i = 0; i < storages; i++) {
    if (!storage_pointers[i])
      continue;
    if (storage_pointers[i]->number == drive)
      return storage_pointers[i];
  }
  return NULL;
}

void devman_add_storage(DevmanStorageType type, void *driver_data,
                        bool (*read)(void *, u64, u32, void *),
                        bool (*write)(void *, u64, u32, void *),
                        bool (*check_attached)(void *)) {
  spinlock(&storage_lock);
  if (!storage_pointers) {
    storage_pointers = kzalloc(sizeof(void *));
    storages = 1;
  }

  DevmanStorage *s = kmalloc(sizeof(*s));
  s->type = type;
  s->driver_data = driver_data;
  s->read = read;
  s->write = write;
  s->check_attached = check_attached;
  s->number = highest_storage++;

  u64 failed = 0;
  for (u64 i = 0; i < storages; i++) {
    if (!storage_pointers[i]) {
      storage_pointers[i] = s;
      break;
    }
    failed++;
  }
  if (failed == storages) {
    storage_pointers =
        krealloc(storage_pointers, storages * 8, (storages + 1) * 8);
    storages++;
    storage_pointers[storages - 1] = s;
  }
  log(LOGLEVEL_INFO, "Registered storage device. Scanning partitions...");
  DevmanStorageAccessHandle ah;
  ah.partition_index = 0;
  ah.storage_index = highest_storage - 1;
  spinunlock(&storage_lock);
  gpt_init(ah);
}

void devman_add_partition(DevmanStorageAccessHandle handle, u64 start, u64 end,
                          u64 id) {
  if (handle.partition_index)
    kernel_panic_error("Attempted to add a partition to a partition");
  DevmanStorage *storage = _devman_get_storage(handle.storage_index);
  spinlock(&storage_lock);
  DevmanStoragePartition *partition =
      (DevmanStoragePartition *)kmalloc(sizeof(*partition));
  partition->id = id;
  partition->start = start;
  partition->end = end;
  u64 failed = 0;
  for (u64 i = 0; i < storage->partition_count; i++) {
    if (!storage->partitions[i]) {
      storage->partitions[i] = partition;
      break;
    }
    failed++;
  }
  if (failed == storage->partition_count) {
    storage->partitions =
        krealloc(storage->partitions, storage->partition_count * 8,
                 (storage->partition_count + 1) * 8);
    storage->partition_count++;
    storage->partitions[storage->partition_count - 1] = partition;
  }
  log(LOGLEVEL_INFO, "Registered partition...");
  spinunlock(&storage_lock);
}

bool devman_read(DevmanStorageAccessHandle handle, u64 sector, u32 count,
                 void *buffer) {
  DevmanStorage *s = _devman_get_storage(handle.storage_index);
  if (!s)
    return false;
  DevmanStoragePartition p =
      _devman_get_partition(handle.storage_index, handle.partition_index);
  if (!p.end)
    return false;
  return s->read(s->driver_data, sector, count, buffer);
}

bool devman_write(DevmanStorageAccessHandle handle, u64 sector, u32 count,
                  void *buffer) {
  DevmanStorage *s = _devman_get_storage(handle.storage_index);
  if (!s)
    return false;
  DevmanStoragePartition p =
      _devman_get_partition(handle.storage_index, handle.partition_index);
  if (!p.end)
    return false;
  return s->write(s->driver_data, sector, count, buffer);
}
