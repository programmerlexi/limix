#pragma once

#include <boot/limine.h>
#include <io/serial/serial.h>

/* This is the kernel config */

#define USED_PAGING_MODE LIMINE_PAGING_MODE_X86_64_4LVL
#define DEFAULT_COM COM1

#define KERNEL_NAME "limix"
#define KERNEL_MAJ "0"
#define KERNEL_MIN "0"
#define KERNEL_PATCH "1"
