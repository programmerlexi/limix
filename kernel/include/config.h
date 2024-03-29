#pragma once

#include <boot/limine.h>
#include <io/serial/serial.h>

/* This is the kernel config */

#define USED_PAGING_MODE LIMINE_PAGING_MODE_X86_64_4LVL
#define DEFAULT_COM COM1

#define MAX_DRMS 1

#define KERNEL_NAME "limix"
#define KERNEL_MAJ "0"
#define KERNEL_MIN "0"
#define KERNEL_PATCH "1"
#define KERNEL_RELEASE "dev"
#define KERNEL_TYPE "core"

#define CONFIG_KEYBOARD_MODE_POLL 0

#define CONFIG_KEYBOARD_MODE CONFIG_KEYBOARD_MODE_POLL

#define CONFIG_HEAP_INITIAL_PAGES 16

#define PATHSEP '/'

#define LOGLEVEL_DEBUG 3
#define LOGLEVEL_INFO 2
#define LOGLEVEL_WARN 1
#define LOGLEVEL_ERROR 0

#define LOGLEVEL LOGLEVEL_DEBUG

#define DRM_WRITETHROUGH
