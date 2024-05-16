#pragma once

#include "types.h"
#include <stdbool.h>

#define config_get_or(path, def, accessfunc)                                   \
  (config_has(path) ? accessfunc(path) : def)

typedef struct config_path {
  u64 module;
  u64 submodule;
  u64 variable;
} config_path_t;

typedef struct config_variable {
  u64 variable;
  u8 type;
  union {
    char *string;
    u64 integer;
    char character;
  } value;
  struct config_variable *next;
} config_variable_t;
typedef struct config_submodule {
  u64 submodule;
  config_variable_t *variables;
  struct config_submodule *next;
} config_submodule_t;
typedef struct config_module {
  u64 module;
  config_submodule_t *submodules;
  struct config_module *next;
} config_module_t;

void config_init();
char *config_get_string(config_path_t path);
u64 config_get_integer(config_path_t path);
char config_get_char(config_path_t path);

void config_set_string(config_path_t path, char *v);
void config_set_integer(config_path_t path, u64 i);
void config_set_char(config_path_t path, char c);

bool config_has(config_path_t path);
