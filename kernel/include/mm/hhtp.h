#pragma once

#include <stdint.h>

extern uint64_t hhaddr;

#define HHDM(addr) ((uint64_t)addr | hhaddr)
#define PHY(addr) ((uint64_t)addr & (~hhaddr))
