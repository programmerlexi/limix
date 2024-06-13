#pragma once

#include "libk/types.h"

void irq_init();
void irq_register(void (*handler)(), u8 irq);
