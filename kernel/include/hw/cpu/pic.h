#pragma once

#include "libk/types.h"

#define PIC_MASTER_CMD 0x20
#define PIC_MASTER_DATA 0x21

#define PIC_SLAVE_CMD 0xa0
#define PIC_SLAVE_DATA 0xa1

#define ICW1_ICW4 0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT 0x10

#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER 0x0c
#define ICW4_SFNM 0x10

#define PIC_READ_IRR 0x0a
#define PIC_READ_ISR 0x0b

#define PIC_EOI 0x20

void pic_send_eoi(u8 irq);
void pic_init();
void pic_remap(i32 offset0, i32 offset1);
void pic_mask(u8 irq);
void pic_unmask(u8 irq);
void pic_disable();
u16 pic_get_irr();
u16 pic_get_isr();
