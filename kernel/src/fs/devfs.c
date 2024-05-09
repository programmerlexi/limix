#include <debug.h>
#include <fs/devfs.h>
#include <fs/vfs.h>
#include <kernel.h>
#include <mm/heap.h>
#include <stdint.h>
#include <utils/errors.h>
#include <utils/memory/memory.h>
#include <utils/memory/safety.h>
#include <utils/strings/xstr.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "devfs"

static device_t *devices;
static vfs_t *devfs;
static uint64_t dev_count;

static int zero_r(void *p, uint64_t o, uint64_t s, char *b) {
  memset(b, 0, s);
  return E_SUCCESS;
}
static int zero_w(void *p, uint64_t o, uint64_t s, char *b) {
  return E_SUCCESS;
}

void devfs_init() {
  devfs_create("null", zero_r, zero_w, NULL);
  devfs_create("zero", zero_r, zero_w, NULL);
}
void devfs_create(char *name, int (*read)(void *, uint64_t, uint64_t, char *),
                  int (*write)(void *, uint64_t, uint64_t, char *),
                  void *data) {
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
  nullsafe_error(device, "Out of memory");
  device->read = read;
  device->write = write;
  device->next = NULL;
  device->name = to_xstr(name);
  device->data = data;
  dev_count++;
}

static int _devfs_read(uint64_t o, uint64_t s, char *b, file_t *f) {
  device_t *d = f->data;
  return d->read(d->data, o, s, b);
}
static int _devfs_write(uint64_t o, uint64_t s, char *b, file_t *f) {
  device_t *d = f->data;
  return d->write(d->data, o, s, b);
}

void devfs_reload() {
  nullsafe(devfs);
  if (!devfs->root)
    devfs->root = malloc(sizeof(directory_t));
  nullsafe_error(devfs->root, "Out of memory");
  devfs->root->file_count = dev_count;
  if (devfs->root->files)
    free(devfs->root->files);
  devfs->root->files = malloc(sizeof(void *) * dev_count);
  device_t *c = devices;
  for (uint64_t i = 0; i < dev_count; i++) {
    devfs->root->files[i] = malloc(sizeof(file_t));
    nullsafe_error(devfs->root->files[i], "Out of memory");
    devfs->root->files[i]->name = c->name;
    devfs->root->files[i]->read = _devfs_read;
    devfs->root->files[i]->write = _devfs_write;
    devfs->root->files[i]->data = c;
    devfs->root->files[i]->size = 0;
    c = c->next;
  }
}
void devfs_bind(vfs_t *fs) { devfs = fs; }
