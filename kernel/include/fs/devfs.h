#pragma once

#include <fs/vfs.h>
#include <stdint.h>
#include <utils/strings/xstr.h>

typedef struct device {
  xstr_t name;
  int (*read)(void *, uint64_t, uint64_t, char *);
  int (*write)(void *, uint64_t, uint64_t, char *);
  void *data;
  struct device *next;
} device_t;

void devfs_init();
void devfs_create(char *name, int (*read)(void *, uint64_t, uint64_t, char *),
                  int (*write)(void *, uint64_t, uint64_t, char *), void *data);
void devfs_reload();
void devfs_bind(vfs_t *fs);
