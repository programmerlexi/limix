#pragma once

#include <stdint.h>

extern uint64_t hhaddr;

#define HHDM(addr) ((uint64_t)addr + hhaddr)
