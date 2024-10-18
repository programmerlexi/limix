#pragma once

#include <libk/types.h>
#include <stdbool.h>

#define config_get_or(path, def, accessfunc)                                   \
  (config_has(path) ? accessfunc(path) : def)

enum ConfigPathEnum {
  CONFIG_KERNEL_NONE,
  CONFIG_VT_SCROLL,
  CONFIG_VFS_PATHSEP,
};

typedef enum ConfigPathEnum ConfigPath;

typedef struct ConfigVariableStruct {
  ConfigPath path;
  u8 type;
  union {
    char *string;
    u64 integer;
    char character;
  } value;
  struct ConfigVariableStruct *next;
} ConfigVariable;

void config_init();
char *config_get_string(ConfigPath path);
u64 config_get_integer(ConfigPath path);
char config_get_char(ConfigPath path);

void config_set_string(ConfigPath path, char *v);
void config_set_integer(ConfigPath path, u64 i);
void config_set_char(ConfigPath path, char c);

bool config_has(ConfigPath path);
