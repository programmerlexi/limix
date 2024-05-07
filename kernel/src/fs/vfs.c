#include <config.h>
#include <debug.h>
#include <fs/devfs.h>
#include <fs/vfs.h>
#include <math/lib.h>
#include <mm/heap.h>
#include <stdint.h>
#include <utils/errors.h>
#include <utils/memory/heap_wrap.h>
#include <utils/strings/strings.h>
#include <utils/strings/xstr.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "vfs"

static vfs_t *filesystems;
static uint64_t fs_count;

void vfs_init() { fs_count = 0; }

vfs_t *vfs_make(char *name) {
  debugf("Making mount '%s'", name);
  vfs_t **a = &filesystems;
  vfs_t *c = filesystems;
  while (c) {
    a = &c->next;
    c = c->next;
  }
  *a = malloc(sizeof(vfs_t));
  (*a)->name = to_xstr(name);
  fs_count++;
  return *a;
}

vfs_t *vfs_fs(char *name) {
  vfs_t *c = filesystems;
  while (c) {
    if (strlen(name) == c->name.length)
      if (strncmp(c->name.cstr, name, min(strlen(c->name.cstr), strlen(name))))
        return c;
    c = c->next;
  }
  return c;
}
int vfs_find_directory(directory_t **result, char *path) {
  uint64_t ft;
  int tr = vfs_type(path, &ft);
  if (tr != E_SUCCESS)
    return tr;
  if (ft != 0)
    return E_NOENT;

  if (!strlen(path))
    return E_INVOPT;

  char *fs_name = clone(path, strnext(path, PATHSEP));
  vfs_t *fs = vfs_fs(fs_name);
  free(fs_name);
  if (!fs)
    return E_NOENT;
  directory_t *d = fs->root;
  uint64_t it = 0;
  while (strlen(path) > strnext(path, PATHSEP)) {
    if (it)
      return E_NOENT;
    it = 0;
    path += strnext(path, PATHSEP) + 1;
    char *name = clone(path, min(strnext(path, PATHSEP), strlen(path)));
    bool found = false;
    for (uint64_t i = 0; i < d->directory_count; i++) {
      if (strlen(d->directories[i]->name.cstr) != strlen(name))
        continue;
      if (strncmp(d->directories[i]->name.cstr, name, strlen(name))) {
        d = d->directories[i];
        free(name);
        found = true;
        *result = d->directories[i];
        return E_SUCCESS;
      }
    }
    if (found)
      continue;
    for (uint64_t i = 0; i < d->file_count; i++) {
      if (strlen(d->files[i]->name.cstr) != strlen(name))
        continue;
      if (strncmp(d->files[i]->name.cstr, name, strlen(name))) {
        free(name);
        found = true;
        it = 1;
        break;
      }
    }
    if (found)
      continue;
    free(name);
    return E_NOENT;
  }
  return E_INVOP;
}
int vfs_find_file(file_t **result, char *path) {
  uint64_t ft;
  int tr = vfs_type(path, &ft);
  if (tr != E_SUCCESS)
    return tr;
  if (ft != 1)
    return E_NOENT;

  if (!strlen(path))
    return E_INVOPT;

  char *fs_name = clone(path, strnext(path, PATHSEP));
  vfs_t *fs = vfs_fs(fs_name);
  free(fs_name);
  if (!fs)
    return E_NOENT;
  directory_t *d = fs->root;
  uint64_t it = 0;
  while (strlen(path) > strnext(path, PATHSEP)) {
    if (it)
      return E_NOENT;
    it = 0;
    path += strnext(path, PATHSEP) + 1;
    char *name = clone(path, min(strnext(path, PATHSEP), strlen(path)));
    bool found = false;
    for (uint64_t i = 0; i < d->directory_count; i++) {
      if (strlen(d->directories[i]->name.cstr) != strlen(name))
        continue;
      if (strncmp(d->directories[i]->name.cstr, name, strlen(name))) {
        d = d->directories[i];
        free(name);
        found = true;
        break;
      }
    }
    if (found)
      continue;
    for (uint64_t i = 0; i < d->file_count; i++) {
      if (strlen(d->files[i]->name.cstr) != strlen(name))
        continue;
      if (strncmp(d->files[i]->name.cstr, name, strlen(name))) {
        free(name);
        found = true;
        it = 1;
        *result = d->files[i];
        return E_SUCCESS;
      }
    }
    if (found)
      continue;
    free(name);
    return E_NOENT;
  }
  return E_INVOP;
}

int vfs_type(char *path, uint64_t *type) {
  if (!strlen(path))
    return E_INVOPT;
  char *fs_name = clone(path, strnext(path, PATHSEP));
  vfs_t *fs = vfs_fs(fs_name);
  free(fs_name);
  if (!fs)
    return E_NOENT;
  directory_t *d = fs->root;
  uint64_t it = 0;
  while (strlen(path) > strnext(path, PATHSEP)) {
    if (it)
      return E_NOENT;
    it = 0;
    path += strnext(path, PATHSEP) + 1;
    char *name = clone(path, min(strnext(path, PATHSEP), strlen(path)));
    bool found = false;
    for (uint64_t i = 0; i < d->directory_count; i++) {
      if (strlen(d->directories[i]->name.cstr) != strlen(name))
        continue;
      if (strncmp(d->directories[i]->name.cstr, name, strlen(name))) {
        d = d->directories[i];
        free(name);
        found = true;
        break;
      }
    }
    if (found)
      continue;
    for (uint64_t i = 0; i < d->file_count; i++) {
      if (strlen(d->files[i]->name.cstr) != strlen(name))
        continue;
      if (strncmp(d->files[i]->name.cstr, name, strlen(name))) {
        free(name);
        found = true;
        it = 1;
        break;
      }
    }
    if (found)
      continue;
    free(name);
    return E_NOENT;
  }
  *type = it;
  return E_SUCCESS;
}
