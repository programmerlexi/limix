#pragma once

#include <stdint.h>

extern uintptr_t hhaddr;

#define HHDM(addr) ((uintptr_t)addr | hhaddr)
#define PHY(addr) ((uintptr_t)addr & (~hhaddr))
