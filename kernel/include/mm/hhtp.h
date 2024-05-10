#pragma once

#include <stdint.h>

extern uintptr_t g_hhaddr;

#define HHDM(addr)                                                             \
  ((uintptr_t)addr < g_hhaddr ? ((uintptr_t)addr + g_hhaddr) : (uintptr_t)addr)
#define PHY(addr)                                                              \
  ((uintptr_t)addr >= g_hhaddr ? ((uintptr_t)addr - g_hhaddr) : (uintptr_t)addr)
