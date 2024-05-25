#pragma once

#include "libk/types.h"
#include <stdbool.h>

#define config_get_or(path, def, accessfunc)                                   \
  (config_has(path) ? accessfunc(path) : def)

enum config_path {
  CONFIG_VT_SCROLL,
  CONFIG_VFS_PATHSEP,
};

typedef enum config_path config_path_t;

typedef struct config_variable {
  config_path_t path;
  u8 type;
  union {
    char *string;
    u64 integer;
    char character;
  } value;
  struct config_variable *next;
} config_variable_t;

void config_init();
char *config_get_string(config_path_t path);
u64 config_get_integer(config_path_t path);
char config_get_char(config_path_t path);

void config_set_string(config_path_t path, char *v);
void config_set_integer(config_path_t path, u64 i);
void config_set_char(config_path_t path, char c);

bool config_has(config_path_t path);
