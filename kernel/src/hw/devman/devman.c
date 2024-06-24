#include "kernel/hw/devman/devman.h"
#include "kernel/debug.h"
#include "kernel/fs/gpt.h"
#include "kernel/mm/heap.h"
#include "libk/utils/memory/heap_wrap.h"
#include <stddef.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "devman"

static devman_storage_t **storage_pointers;
static u64 storages;
static u64 highest_storage;

void devman_init() {
  storage_pointers = NULL;
  storages = 0;
  highest_storage = 0;
}

void devman_add_storage(devman_storage_type_t type, void *driver_data,
                        bool (*read)(void *, u64, u32, void *),
                        bool (*write)(void *, u64, u32, void *),
                        bool (*check_attached)(void *)) {
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
  gpt_init(s->driver_data, s->read);
}
