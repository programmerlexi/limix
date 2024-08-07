#pragma once

#include "libk/types.h"
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

void devman_init();
void devman_add_storage(DevmanStorageType type, void *driver_data,
                        bool (*read)(void *, u64, u32, void *),
                        bool (*write)(void *, u64, u32, void *),
                        bool (*check_attached)(void *));
bool devman_read(DevmanStorageAccessHandle handle, u64 sector, u32 count,
                 void *buffer);
bool devman_write(DevmanStorageAccessHandle handle, u64 sector, u32 count,
                  void *buffer);
