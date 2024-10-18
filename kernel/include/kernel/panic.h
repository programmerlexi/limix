#pragma once

static inline void kpanic() { asm("int $0x40"); }

void panic_init();
