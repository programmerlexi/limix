#pragma once

#include "boot/limine.h"
#include "config/config.h"
#include "debug.h"
#include "io/serial/serial.h"

/* This is the kernel config */

#define USED_PAGING_MODE LIMINE_PAGING_MODE_X86_64_4LVL
#define DEFAULT_COM COM1

#define MAX_DRMS 1

#define KERNEL_MAJ "0"
#define KERNEL_MIN "0"
#define KERNEL_PATCH "1"

#define CONFIG_KEYBOARD_MODE_POLL 0

#define CONFIG_KEYBOARD_MODE CONFIG_KEYBOARD_MODE_POLL

#define CONFIG_HEAP_INITIAL_PAGES 16

#define CONFIG_SCROLL_STEP                                                     \
  config_get_or(((config_path_t){0x726e656c, 0x7674, 0x726f6c6c}), 1,          \
                config_get_integer)

#define PATHSEP                                                                \
  config_get_or(((config_path_t){0x726e656c, 'vfs', 'sep'}), '/',              \
                config_get_char)

#define LOGLEVEL LOGLEVEL_DEBUG
// #define LOG_SERIAL

#define DRM_WRITETHROUGH

#define TASK_LIMIT 128
