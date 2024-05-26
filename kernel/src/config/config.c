#include "kernel/config/config.h"
#include "kernel/mm/heap.h"
#include <stddef.h>

static config_variable_t *_variables;

static config_variable_t *_config_resolve(config_path_t path) {
  config_variable_t *c = _variables;
  while (c) {
    if (c->path == path)
      return c;
    c = c->next;
  }
  return NULL;
}

static config_variable_t *_config_get_or_create(config_path_t p) {
  if (!_variables)
    config_init();
  config_variable_t *v = _config_resolve(p);
  if (v)
    return v;
  v = kmalloc(sizeof(config_variable_t));
  v->path = p;
  v->type = 0xff;
  v->next = _variables->next;
  _variables->next = v;
  return _config_resolve(p);
}

u64 config_get_integer(config_path_t path) {
  config_variable_t *v = _config_resolve(path);
  if (v->type != 1)
    return 0;
  return v->value.integer;
}

char *config_get_string(config_path_t path) {
  config_variable_t *v = _config_resolve(path);
  if (v->type != 0)
    return "<UNKNOWN>";
  return v->value.string;
}

char config_get_char(config_path_t path) {
  config_variable_t *v = _config_resolve(path);
  if (v->type != 2)
    return '\x01';
  return v->value.character;
}

void config_set_integer(config_path_t path, u64 i) {
  config_variable_t *v = _config_get_or_create(path);
  if (v->type == 0xff)
    v->type = 1;
  else if (v->type != 1)
    return;
  v->value.integer = i;
}
void config_set_string(config_path_t path, char *s) {
  config_variable_t *v = _config_get_or_create(path);
  if (v->type == 0xff)
    v->type = 0;
  else if (v->type != 0)
    return;
  v->value.string = s;
}
void config_set_char(config_path_t path, char c) {
  config_variable_t *v = _config_get_or_create(path);
  if (v->type == 0xff)
    v->type = 2;
  else if (v->type != 2)
    return;
  v->value.character = c;
}

bool config_has(config_path_t path) { return _config_resolve(path) != NULL; }

void config_init() {
  _variables = kmalloc(sizeof(config_variable_t));
  _variables->path = CONFIG_KERNEL_NONE;
  _variables->type = -1;
}
