#include "kernel/hw/devman/devman.h"
#include "kernel/debug.h"
#include "kernel/fs/gpt.h"
#include "kernel/mm/heap.h"
#include "libk/ipc/spinlock.h"
#include "libk/utils/memory/heap_wrap.h"
#include <stddef.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "devman"

static devman_storage_t **storage_pointers;
static u64 storages;
static u64 highest_storage;
static u32 storage_lock = 1;

void devman_init() {
  log(LOGLEVEL_INFO, "Initializing devman...");
  storage_pointers = NULL;
  storages = 0;
  highest_storage = 0;
  spinunlock(&storage_lock);
}

void devman_add_storage(devman_storage_type_t type, void *driver_data,
                        bool (*read)(void *, u64, u32, void *),
                        bool (*write)(void *, u64, u32, void *),
                        bool (*check_attached)(void *)) {
  spinlock(&storage_lock);
  if (!storage_pointers) {
    storage_pointers = kcalloc(sizeof(void *));
    storages = 1;
  }

  devman_storage_t *s = kmalloc(sizeof(*s));
  s->type = type;
  s->driver_data = driver_data;
  s->read = read;
  s->write = write;
  s->check_attached = check_attached;
  s->number = highest_storage++;

  u64 failed = 0;
  for (u64 i = 0; i < storages; i++) {
    if (!storage_pointers[i]) {
      storage_pointers[i] = s;
      break;
    }
    failed++;
  }
  if (failed == storages) {
    storage_pointers =
        krealloc(storage_pointers, storages * 8, (storages + 1) * 8);
    storages++;
    storage_pointers[storages - 1] = s;
  }
  log(LOGLEVEL_INFO, "Registered storage device. Scanning partitions...");
  spinunlock(&storage_lock);
  devman_storage_access_handle_t ah;
  ah.partition_index = 0;
  ah.storage_index = highest_storage - 1;
  gpt_init(ah);
}

static devman_storage_partition_t _devman_get_partition(u64 drive,
                                                        u64 partition) {
  devman_storage_partition_t r;
  r.id = 0;
  r.start = 0;
  r.end = 0;
  if (!partition) {
    r.id = 0;
    r.start = 0;
    r.end = -1;
  } else {
    if (!(drive < highest_storage))
      return r;
    for (u64 i = 0; i < storages; i++) {
      if (!storage_pointers[i])
        continue;
      if (storage_pointers[i]->number == drive) {
        if (!storage_pointers[i]->partitions)
          break;
        for (u64 j = 0; j < storage_pointers[i]->partition_count; j++) {
          if (!storage_pointers[i]->partitions[j])
            continue;
          if (storage_pointers[i]->partitions[j]->id == partition) {
            r.start = storage_pointers[i]->partitions[j]->start;
            r.end = storage_pointers[i]->partitions[j]->end;
            r.id = storage_pointers[i]->partitions[j]->id;
            break;
          }
        }
        break;
      }
    }
  }
  return r;
}

static devman_storage_t *_devman_get_storage(u64 drive) {
  if (!storage_pointers)
    return NULL;
  for (u64 i = 0; i < storages; i++) {
    if (!storage_pointers[i])
      continue;
    if (storage_pointers[i]->number == drive)
      return storage_pointers[i];
  }
  return NULL;
}

bool devman_read(devman_storage_access_handle_t handle, u64 sector, u32 count,
                 void *buffer) {
  devman_storage_t *s = _devman_get_storage(handle.storage_index);
  if (!s)
    return false;
  devman_storage_partition_t p =
      _devman_get_partition(handle.storage_index, handle.partition_index);
  if (!p.end)
    return false;
  return s->read(s->driver_data, sector, count, buffer);
}

bool devman_write(devman_storage_access_handle_t handle, u64 sector, u32 count,
                  void *buffer) {
  devman_storage_t *s = _devman_get_storage(handle.storage_index);
  if (!s)
    return false;
  devman_storage_partition_t p =
      _devman_get_partition(handle.storage_index, handle.partition_index);
  if (!p.end)
    return false;
  return s->write(s->driver_data, sector, count, buffer);
}
