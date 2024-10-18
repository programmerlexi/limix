#pragma once

#include <kernel/hw/pci/pci.h>
#include <libk/types.h>
#include <stdbool.h>

typedef enum {
  HDD,
  CD,
} DevmanStorageType;

typedef struct {
  u64 start;
  u64 end;
  u64 id;
} DevmanStoragePartition;

typedef struct {
  DevmanStorageType type;
  u64 number;
  void *driver_data;
  bool (*read)(void *, u64, u32, void *);
  bool (*write)(void *, u64, u32, void *);
  bool (*check_attached)(void *);
  u64 partition_count;
  DevmanStoragePartition **partitions;
} DevmanStorage;

typedef struct {
  u64 storage_index;
  u64 partition_index;
} DevmanStorageAccessHandle;

enum PcieDriverAddressTypeEnum {
  VENDOR,
  VENDORDEVICE,
  CLASS,
  CLASSSUBCLASS,
  CLASSSUBCLASSPROGIF,
};

typedef struct PcieDriverHandleStruct {
  enum PcieDriverAddressTypeEnum address_type;
  u16 address[3];
  bool (*init)(PciType0 *device);
  struct PcieDriverHandleStruct *next;
} PcieDriverHandle;

void devman_init();
void devman_add_drivers();
void devman_enumerate();
void devman_register_driver(enum PcieDriverAddressTypeEnum address_type,
                            u16 addr0, u16 addr1, u16 addr2,
                            bool (*init)(PciType0 *device));

void devman_add_storage(DevmanStorageType type, void *driver_data,
                        bool (*read)(void *, u64, u32, void *),
                        bool (*write)(void *, u64, u32, void *),
                        bool (*check_attached)(void *));
void devman_add_partition(DevmanStorageAccessHandle handle, u64 start, u64 end,
                          u64 id);
bool devman_read(DevmanStorageAccessHandle handle, u64 sector, u32 count,
                 void *buffer);
bool devman_write(DevmanStorageAccessHandle handle, u64 sector, u32 count,
                  void *buffer);
