#pragma once

#include "libk/types.h"

#define PIT_BASE 0x40

#define PIT_CHANNEL0 PIT_BASE + 0
#define PIT_CHANNEL1 PIT_BASE + 1
#define PIT_CHANNEL2 PIT_BASE + 2
#define PIT_CMD PIT_BASE + 3

#define PIT_MODE_CHANNEL0 (0 << 6)
#define PIT_MODE_CHANNEL1 (1 << 6)
#define PIT_MODE_CHANNEL2 (2 << 6)

#define PIT_MODE_ACCESS_LATCH_COUNT_VALUE (0 << 4)
#define PIT_MODE_ACCESS_LOW (1 << 4)
#define PIT_MODE_ACCESS_HIGH (2 << 4)
#define PIT_MODE_ACCESS_LOW_HIGH (3 << 4)

#define PIT_MODE_TERMINAL_INT (0 << 1)
#define PIT_MODE_HARDWARE_RETRIGGER_ONESHOT (1 << 1)
#define PIT_MODE_RATE (2 << 1)
#define PIT_MODE_SQUARE_WAVE (3 << 1)
#define PIT_MODE_SOFTWARE_TRIGGER_STROBE (4 << 1)
#define PIT_MODE_HARDWARE_TRIGGER_STROBE (5 << 1)

#define PIT_MODE_BCD 1

#define PIT_BASE_FREQ 1193182

void pit_init();
void pit_set_freq(u64 freq);
void pit_set_divisor(u64 divisor);
