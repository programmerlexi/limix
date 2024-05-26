#pragma once

#include "libk/types.h"

#define translate_to_virt(p) (g_virtual_base + (p - g_physical_base))
#define translate_to_phys(p) (g_physical_base + (p - g_virtual_base))

extern u64 g_virtual_base;
extern u64 g_physical_base;
