#pragma once

#include "kernel/debug.h"
#include "kernel/io/serial/serial.h"
#include "limine.h"

/* This is the kernel config */

#define USED_PAGING_MODE LIMINE_PAGING_MODE_X86_64_4LVL
#define DEFAULT_COM COM1

#define MAX_DRMS 1

#define KERNEL_MAJ 0
#define KERNEL_MIN 0
#define KERNEL_PATCH 1

#define CONFIG_KEYBOARD_MODE_POLL 0

#define CONFIG_KEYBOARD_MODE CONFIG_KEYBOARD_MODE_POLL

#define CONFIG_HEAP_INITIAL_PAGES 2

#ifdef CONFIG_DYNCONF
#define CONFIG_SCROLL_STEP                                                     \
  config_get_or(CONFIG_VT_SCROLL, 4, config_get_integer)

#define PATHSEP config_get_or(CONFIG_VFS_PATHSEP, '/', config_get_char)
#else
#define CONFIG_SCROLL_STEP 4
#define PATHSEP '/'
#endif

#define LOGLEVEL LOGLEVEL_INFO
#define LOGLEVEL_SERIAL LOGLEVEL_WARN0

// #define LOG_SERIAL

#define DRM_WRITETHROUGH

/*#define PCIE_COLOR*/

#define TASK_LIMIT 128
