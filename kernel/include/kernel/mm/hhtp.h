#pragma once

#include <libk/types.h>

extern uptr g_hhaddr;

#define HHDM(addr)                                                             \
  ((uptr)addr < g_hhaddr ? ((uptr)addr + g_hhaddr) : (uptr)addr)
#define PHY(addr)                                                              \
  ((uptr)addr >= g_hhaddr ? ((uptr)addr - g_hhaddr) : (uptr)addr)
