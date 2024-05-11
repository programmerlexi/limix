#pragma once
#include "types.h"
#include <stdint.h>

void outb(u16 port, u8 val);
void outw(u16 port, u16 val);
void outd(u16 port, u32 val);

u8 inb(u16 port);
u16 inw(u16 port);
u32 ind(u16 port);

static inline void io_wait(void) { inb(0x80); }
