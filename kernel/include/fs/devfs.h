#pragma once

#include <fs/vfs.h>
#include <stdint.h>
#include <utils/strings/xstr.h>

typedef struct device {
  xstr_t name;
  int (*read)(uint64_t, uint64_t, char *);
  int (*write)(uint64_t, uint64_t, char *);
  struct device *next;
} device_t;

void devfs_init();
void devfs_create(char *name, int (*read)(uint64_t, uint64_t, char *),
                  int (*write)(uint64_t, uint64_t, char *));
void devfs_reload();
void devfs_bind(vfs_t *fs);
