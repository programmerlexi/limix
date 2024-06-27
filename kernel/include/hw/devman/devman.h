#pragma once

#include "libk/types.h"
#include <stdbool.h>

typedef enum {
  HDD,
  CD,
} devman_storage_type_t;

typedef struct {
  u64 start;
  u64 end;
  u64 id;
} devman_storage_partition_t;

typedef struct {
  devman_storage_type_t type;
  u64 number;
  void *driver_data;
  bool (*read)(void *, u64, u32, void *);
  bool (*write)(void *, u64, u32, void *);
  bool (*check_attached)(void *);
  u64 partition_count;
  devman_storage_partition_t **partitions;
} devman_storage_t;

typedef struct {
  u64 storage_index;
  u64 partition_index;
} devman_storage_access_handle_t;

void devman_init();
void devman_add_storage(devman_storage_type_t type, void *driver_data,
                        bool (*read)(void *, u64, u32, void *),
                        bool (*write)(void *, u64, u32, void *),
                        bool (*check_attached)(void *));
bool devman_read(devman_storage_access_handle_t handle, u64 sector, u32 count,
                 void *buffer);
bool devman_write(devman_storage_access_handle_t handle, u64 sector, u32 count,
                  void *buffer);
