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
static u64 dev_count;

static int zero_r(void *p, u64 o, u64 s, char *b) {
  memset(b, 0, s);
  return E_SUCCESS;
}
static int zero_w(void *p, u64 o, u64 s, char *b) { return E_SUCCESS; }

void devfs_init() {
  devfs_create("null", zero_r, zero_w, NULL);
  devfs_create("zero", zero_r, zero_w, NULL);
}
void devfs_create(char *name, int (*read)(void *, u64, u64, char *),
                  int (*write)(void *, u64, u64, char *), void *data) {
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

static int _devfs_read(u64 o, u64 s, char *b, file_t *f) {
  device_t *d = f->data;
  return d->read(d->data, o, s, b);
}
static int _devfs_write(u64 o, u64 s, char *b, file_t *f) {
  device_t *d = f->data;
  return d->write(d->data, o, s, b);
}

void _devfs_clear_files() {
  for (u64 i = 0; i < devfs->root->file_count; i++) {
    if (devfs->root->files[i]->data)
      free(devfs->root->files[i]->data);
    free(devfs->root->files[i]);
  }
  free(devfs->root->files);
}

void devfs_reload() {
  nullsafe(devfs);
  if (!devfs->root)
    devfs->root = malloc(sizeof(directory_t));
  nullsafe_error(devfs->root, "Out of memory");
  if (devfs->root->files)
    _devfs_clear_files();
  devfs->root->file_count = dev_count;
  devfs->root->files = malloc(sizeof(void *) * dev_count);
  device_t *c = devices;
  for (u64 i = 0; i < dev_count; i++) {
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
