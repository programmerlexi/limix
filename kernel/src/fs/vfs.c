#include "kernel/fs/vfs.h"
#include "kernel/config.h"
#include "kernel/debug.h"
#include "kernel/mm/heap.h"
#include "libk/math/lib.h"
#include "libk/utils/errors.h"
#include "libk/utils/memory/heap_wrap.h"
#include "libk/utils/memory/safety.h"
#include "libk/utils/strings/strings.h"
#include "libk/utils/strings/xstr.h"
#include <stdbool.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "vfs"

static vfs_t *_filesystems;
static u64 _fs_count;

void vfs_init() { _fs_count = 0; }

vfs_t *vfs_make(char *name) {
  debugf("Making mount '%s'", name);
  vfs_t **a = &_filesystems;
  vfs_t *c = _filesystems;
  while (c) {
    a = &c->next;
    c = c->next;
  }
  *a = (vfs_t *)kmalloc(sizeof(vfs_t));
  nullsafe_error(*a, "Out of memory");
  (*a)->name = to_xstr(name);
  _fs_count++;
  return *a;
}

vfs_t *vfs_fs(char *name) {
  vfs_t *c = _filesystems;
  while (c) {
    if (kstrlen(name) == c->name.length)
      if (kstrncmp(c->name.cstr, name,
                   min(kstrlen(c->name.cstr), kstrlen(name))))
        return c;
    c = c->next;
  }
  return c;
}
i32 vfs_find_directory(directory_t **result, char *path) {
  u64 ft;
  i32 tr = vfs_type(path, &ft);
  if (tr != E_SUCCESS)
    return tr;
  if (ft != 0)
    return E_NOENT;

  if (!kstrlen(path))
    return E_INVOPT;

  char *fs_name = (char *)clone(path, kstrnext(path, PATHSEP));
  vfs_t *fs = vfs_fs(fs_name);
  kfree(fs_name);
  if (!fs)
    return E_NOENT;
  directory_t *d = fs->root;
  u64 it = 0;
  while (kstrlen(path) > kstrnext(path, PATHSEP)) {
    if (it)
      return E_NOENT;
    it = 0;
    path += kstrnext(path, PATHSEP) + 1;
    char *name =
        (char *)clone(path, min(kstrnext(path, PATHSEP), kstrlen(path)));
    bool found = false;
    for (u64 i = 0; i < d->directory_count; i++) {
      if (kstrlen(d->directories[i]->name.cstr) != kstrlen(name))
        continue;
      if (kstrncmp(d->directories[i]->name.cstr, name, kstrlen(name))) {
        d = d->directories[i];
        kfree(name);
        found = true;
        *result = d->directories[i];
        return E_SUCCESS;
      }
    }
    if (found)
      continue;
    for (u64 i = 0; i < d->file_count; i++) {
      if (kstrlen(d->files[i]->name.cstr) != kstrlen(name))
        continue;
      if (kstrncmp(d->files[i]->name.cstr, name, kstrlen(name))) {
        kfree(name);
        found = true;
        it = 1;
        break;
      }
    }
    if (found)
      continue;
    kfree(name);
    return E_NOENT;
  }
  return E_INVOP;
}
i32 vfs_find_file(file_t **result, char *path) {
  u64 ft;
  i32 tr = vfs_type(path, &ft);
  if (tr != E_SUCCESS)
    return tr;
  if (ft != 1)
    return E_NOENT;

  if (!kstrlen(path))
    return E_INVOPT;

  char *fs_name = (char *)clone(path, kstrnext(path, PATHSEP));
  vfs_t *fs = vfs_fs(fs_name);
  kfree(fs_name);
  if (!fs)
    return E_NOENT;
  directory_t *d = fs->root;
  u64 it = 0;
  while (kstrlen(path) > kstrnext(path, PATHSEP)) {
    if (it)
      return E_NOENT;
    it = 0;
    path += kstrnext(path, PATHSEP) + 1;
    char *name =
        (char *)clone(path, min(kstrnext(path, PATHSEP), kstrlen(path)));
    bool found = false;
    for (u64 i = 0; i < d->directory_count; i++) {
      if (kstrlen(d->directories[i]->name.cstr) != kstrlen(name))
        continue;
      if (kstrncmp(d->directories[i]->name.cstr, name, kstrlen(name))) {
        d = d->directories[i];
        kfree(name);
        found = true;
        break;
      }
    }
    if (found)
      continue;
    for (u64 i = 0; i < d->file_count; i++) {
      if (kstrlen(d->files[i]->name.cstr) != kstrlen(name))
        continue;
      if (kstrncmp(d->files[i]->name.cstr, name, kstrlen(name))) {
        kfree(name);
        found = true;
        it = 1;
        *result = d->files[i];
        return E_SUCCESS;
      }
    }
    if (found)
      continue;
    kfree(name);
    return E_NOENT;
  }
  return E_INVOP;
}

i32 vfs_type(char *path, u64 *type) {
  if (!kstrlen(path))
    return E_INVOPT;
  char *fs_name = (char *)clone(path, kstrnext(path, PATHSEP));
  vfs_t *fs = vfs_fs(fs_name);
  kfree(fs_name);
  if (!fs)
    return E_NOENT;
  directory_t *d = fs->root;
  u64 it = 0;
  while (kstrlen(path) > kstrnext(path, PATHSEP)) {
    if (it)
      return E_NOENT;
    it = 0;
    path += kstrnext(path, PATHSEP) + 1;
    char *name =
        (char *)clone(path, min(kstrnext(path, PATHSEP), kstrlen(path)));
    bool found = false;
    for (u64 i = 0; i < d->directory_count; i++) {
      if (kstrlen(d->directories[i]->name.cstr) != kstrlen(name))
        continue;
      if (kstrncmp(d->directories[i]->name.cstr, name, kstrlen(name))) {
        d = d->directories[i];
        kfree(name);
        found = true;
        break;
      }
    }
    if (found)
      continue;
    for (u64 i = 0; i < d->file_count; i++) {
      if (kstrlen(d->files[i]->name.cstr) != kstrlen(name))
        continue;
      if (kstrncmp(d->files[i]->name.cstr, name, kstrlen(name))) {
        kfree(name);
        found = true;
        it = 1;
        break;
      }
    }
    if (found)
      continue;
    kfree(name);
    return E_NOENT;
  }
  *type = it;
  return E_SUCCESS;
}
