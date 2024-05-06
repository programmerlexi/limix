#include "gfx/drm.h"
#include <debug.h>
#include <fs/devfs.h>
#include <fs/vfs.h>
#include <mm/heap.h>
#include <stdint.h>
#include <utils/errors.h>
#include <utils/memory/memory.h>
#include <utils/strings/xstr.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "devfs"

static device_t *devices;
static vfs_t *devfs;
static uint64_t dev_count;

static int zero_r(uint64_t o, uint64_t s, char *b) {
  memset(b, 0, s);
  return E_SUCCESS;
}
static int zero_w(uint64_t o, uint64_t s, char *b) { return E_SUCCESS; }

void devfs_init() {
  devfs_create("null", zero_r, zero_w);
  devfs_create("zero", zero_r, zero_w);
}
void devfs_create(char *name, int (*read)(uint64_t, uint64_t, char *),
                  int (*write)(uint64_t, uint64_t, char *)) {
  debugf("Creating dev node '%s'...", name);
  device_t *device;
  if (!devices)
    device = devices = malloc(sizeof(device_t));
  else {
    device = malloc(sizeof(device_t));
    device_t *c = devices;
    device_t *p = NULL;
    while (c) {
      p = c;
      c = c->next;
    }
    p->next = device;
  }
  device->read = read;
  device->write = write;
  device->next = NULL;
  device->name = to_xstr(name);
  dev_count++;
}
void devfs_reload() {
  if (!devfs->root)
    devfs->root = malloc(sizeof(directory_t));
  devfs->root->file_count = dev_count;
  devfs->root->files =
      malloc(sizeof(void *) * dev_count); // Memory leak, but idc.
  device_t *c = devices;
  for (uint64_t i = 0; i < dev_count; i++) {
    devfs->root->files[i] = malloc(sizeof(file_t));
    devfs->root->files[i]->name = c->name;
    devfs->root->files[i]->read = c->read;
    devfs->root->files[i]->write = c->write;
    devfs->root->files[i]->size = 0;
  }
}
void devfs_bind(vfs_t *fs) { devfs = fs; }
