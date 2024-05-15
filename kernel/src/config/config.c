#include "config/config.h"
#include "defines.h"
#include "mm/heap.h"

static config_module_t *_modules;

static config_module_t *_config_resolve_module(u64 m) {
  config_module_t *c = _modules;
  while (c) {
    if (c->module == m)
      return c;
    c = c->next;
  }
  return NULL;
}
static config_submodule_t *_config_resolve_submodule(config_module_t *m,
                                                     u64 s) {
  config_submodule_t *c = m->submodules;
  while (c) {
    if (c->submodule == s)
      return c;
    c = c->next;
  }
  return NULL;
}
static config_variable_t *_config_resolve_variable(config_submodule_t *s,
                                                   u64 v) {
  config_variable_t *c = s->variables;
  while (c) {
    if (c->variable == v)
      return c;
    c = c->next;
  }
  return NULL;
}

static config_variable_t *_config_resolve(config_path_t p) {
  config_module_t *m;
  config_submodule_t *s;
  if (!(m = _config_resolve_module(p.module)))
    return NULL;
  if (!(s = _config_resolve_submodule(m, p.module)))
    return NULL;
  return _config_resolve_variable(s, p.variable);
}

static config_variable_t *_config_get_or_create(config_path_t p) {
  config_module_t *m = _config_resolve_module(p.module);
  if (!m) {
    config_module_t *c = _modules;
    if (!c)
      config_init();
    else {
      while (c->next)
        c = c->next;
      c->next = m = kmalloc(sizeof(config_module_t));
    }
    m->module = p.module;
  }
  config_submodule_t *s = _config_resolve_submodule(m, p.submodule);
  if (!s) {
    config_submodule_t *c = m->submodules;
    if (!c)
      m->submodules = s = kmalloc(sizeof(config_submodule_t));
    else {
      while (c->next)
        c = c->next;
      c->next = s = kmalloc(sizeof(config_submodule_t));
    }
    s->submodule = p.submodule;
  }
  config_variable_t *v = _config_resolve_variable(s, p.variable);
  if (!v) {
    config_variable_t *c = s->variables;
    if (!c)
      s->variables = v = kmalloc(sizeof(config_variable_t));
    else {
      while (c->next)
        c = c->next;
      c->next = v = kmalloc(sizeof(config_variable_t));
    }
    v->variable = p.variable;
    v->type = -1;
  }
  return v;
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

BOOL config_has(config_path_t path) { return _config_resolve(path) != NULL; }

void config_init() {
  _modules = kmalloc(sizeof(config_module_t));
  config_set_integer((config_path_t){0x726e656c, 0x7674, 0x726f6c6c}, 16);
}
