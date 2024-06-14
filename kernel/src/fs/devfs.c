#include "kernel/fs/devfs.h"
#include "kernel/debug.h"
#include "kernel/fs/vfs.h"
#include "kernel/mm/heap.h"
#include "libk/utils/errors.h"
#include "libk/utils/memory/memory.h"
#include "libk/utils/memory/safety.h"
#include "libk/utils/strings/xstr.h"
#include <stddef.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "devfs"

static device_t *_devices;
static vfs_t *_devfs;
static u64 _dev_count;

static i32 _zero_r(void *p, u64 o, u64 s, char *b) {
  kmemset(b, 0, s);
  return E_SUCCESS;
}
static i32 _zero_w(void *p, u64 o, u64 s, char *b) { return E_SUCCESS; }

void devfs_init() {
  devfs_create("null", _zero_r, _zero_w, NULL);
  devfs_create("zero", _zero_r, _zero_w, NULL);
}
void devfs_create(char *name, i32 (*read)(void *, u64, u64, char *),
                  i32 (*write)(void *, u64, u64, char *), void *data) {
  debugf("Creating dev node '%s'...", name);
  device_t *device;
  if (!_devices)
    device = _devices = kmalloc(sizeof(device_t));
  else {
    device = kmalloc(sizeof(device_t));
    device_t *c = _devices;
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
  _dev_count++;
}

static i32 _devfs_read(u64 o, u64 s, char *b, file_t *f) {
  device_t *d = f->data;
  return d->read(d->data, o, s, b);
}
static i32 _devfs_write(u64 o, u64 s, char *b, file_t *f) {
  device_t *d = f->data;
  return d->write(d->data, o, s, b);
}

static void _devfs_clear_files() {
  for (u64 i = 0; i < _devfs->root->file_count; i++) {
    if (_devfs->root->files[i]->data)
      kfree(_devfs->root->files[i]->data);
    kfree(_devfs->root->files[i]);
  }
  kfree(_devfs->root->files);
}

void devfs_reload() {
  nullsafe(_devfs);
  if (!_devfs->root)
    _devfs->root = kmalloc(sizeof(directory_t));
  if (_devfs->root->files)
    _devfs_clear_files();
  _devfs->root->file_count = _dev_count;
  _devfs->root->files = kmalloc(sizeof(void *) * _dev_count);
  device_t *c = _devices;
  for (u64 i = 0; i < _dev_count; i++) {
    nullsafe(c);
    _devfs->root->files[i] = kmalloc(sizeof(file_t));
    nullsafe_error(_devfs->root->files[i], "Out of memory");
    _devfs->root->files[i]->name = c->name;
    _devfs->root->files[i]->read = _devfs_read;
    _devfs->root->files[i]->write = _devfs_write;
    _devfs->root->files[i]->data = c;
    _devfs->root->files[i]->size = 0;
    c = c->next;
  }
}
void devfs_bind(vfs_t *fs) { _devfs = fs; }
