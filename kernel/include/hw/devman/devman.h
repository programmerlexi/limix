#pragma once

#include "libk/types.h"
#include <stdbool.h>

typedef enum {
  HDD,
  CD,
} devman_storage_type_t;

typedef struct {
  devman_storage_type_t type;
  u64 number;
  void *driver_data;
  bool (*read)(void *, u64, u32, void *);
  bool (*write)(void *, u64, u32, void *);
  bool (*check_attached)(void *);
} devman_storage_t;

void devman_init();
void devman_add_storage(devman_storage_type_t type, void *driver_data,
                        bool (*read)(void *, u64, u32, void *),
                        bool (*write)(void *, u64, u32, void *),
                        bool (*check_attached)(void *));
