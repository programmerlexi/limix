#pragma once

#include <stdbool.h>
#include <stdint.h>

#define config_get_or(path, def, accessfunc)                                   \
  (config_has(path) ? accessfunc(path) : def)

typedef struct config_path {
  uint64_t module;
  uint64_t submodule;
  uint64_t variable;
} config_path_t;

typedef struct config_variable {
  uint64_t variable;
  uint8_t type;
  union {
    char *string;
    uint64_t integer;
    char character;
  } value;
  struct config_variable *next;
} config_variable_t;
typedef struct config_submodule {
  uint64_t submodule;
  config_variable_t *variables;
  struct config_submodule *next;
} config_submodule_t;
typedef struct config_module {
  uint64_t module;
  config_submodule_t *submodules;
  struct config_module *next;
} config_module_t;

void config_init();
char *config_get_string(config_path_t path);
uint64_t config_get_integer(config_path_t path);
char config_get_char(config_path_t path);

void config_set_string(config_path_t path, char *v);
void config_set_integer(config_path_t path, uint64_t i);
void config_set_char(config_path_t path, char c);

bool config_has(config_path_t path);
