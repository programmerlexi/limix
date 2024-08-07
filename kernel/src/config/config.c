#include "kernel/config/config.h"
#include "kernel/mm/heap.h"
#include <stddef.h>

static ConfigVariable *_variables;

static ConfigVariable *_config_resolve(ConfigPath path) {
  ConfigVariable *c = _variables;
  while (c) {
    if (c->path == path)
      return c;
    c = c->next;
  }
  return NULL;
}

static ConfigVariable *_config_get_or_create(ConfigPath p) {
  if (!_variables)
    config_init();
  ConfigVariable *v = _config_resolve(p);
  if (v)
    return v;
  v = kmalloc(sizeof(ConfigVariable));
  v->path = p;
  v->type = 0xff;
  v->next = _variables->next;
  _variables->next = v;
  return _config_resolve(p);
}

u64 config_get_integer(ConfigPath path) {
  ConfigVariable *v = _config_resolve(path);
  if (v->type != 1)
    return 0;
  return v->value.integer;
}

char *config_get_string(ConfigPath path) {
  ConfigVariable *v = _config_resolve(path);
  if (v->type != 0)
    return "<UNKNOWN>";
  return v->value.string;
}

char config_get_char(ConfigPath path) {
  ConfigVariable *v = _config_resolve(path);
  if (v->type != 2)
    return '\x01';
  return v->value.character;
}

void config_set_integer(ConfigPath path, u64 i) {
  ConfigVariable *v = _config_get_or_create(path);
  if (v->type == 0xff)
    v->type = 1;
  else if (v->type != 1)
    return;
  v->value.integer = i;
}
void config_set_string(ConfigPath path, char *s) {
  ConfigVariable *v = _config_get_or_create(path);
  if (v->type == 0xff)
    v->type = 0;
  else if (v->type != 0)
    return;
  v->value.string = s;
}
void config_set_char(ConfigPath path, char c) {
  ConfigVariable *v = _config_get_or_create(path);
  if (v->type == 0xff)
    v->type = 2;
  else if (v->type != 2)
    return;
  v->value.character = c;
}

bool config_has(ConfigPath path) { return _config_resolve(path) != NULL; }

void config_init() {
  _variables = kmalloc(sizeof(ConfigVariable));
  _variables->path = CONFIG_KERNEL_NONE;
  _variables->type = -1;
}
