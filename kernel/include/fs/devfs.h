#pragma once

struct device;

#include <stdint.h>
#include <utils/errors.h>

typedef int ioctl_t;

typedef struct device {
  char *name;
  error_t (*read)(char *buffer, uint64_t count, uint64_t offset);
  error_t (*write)(char *buffer, uint64_t count, uint64_t offset);
  error_t (*ioctl)(ioctl_t ctl);
  struct device *next;
} device_t;

void devfs_init();

void devfs_register_device(
    char *name, error_t (*read)(char *buffer, uint64_t count, uint64_t offset),
    error_t (*write)(char *buffer, uint64_t count, uint64_t offset),
    error_t (*ioctl)(ioctl_t ctl));

void devfs_remove_device(char *name);

#include <fs/vfs.h>
void devfs_mount(struct tnode *node);
