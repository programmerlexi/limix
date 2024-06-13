#include "kernel/hw/pic/pic.h"
#include "kernel/io/pio.h"

void pic_init() {
  pic_remap(0x20, 0x28);
  pic_disable();
}

void pic_remap(i32 offset0, i32 offset1) {
  u8 a0, a1;
  a0 = inb(PIC_MASTER_DATA);
  a1 = inb(PIC_SLAVE_DATA);
  outb(PIC_MASTER_CMD, ICW1_INIT | ICW1_ICW4);
  io_wait();
  outb(PIC_SLAVE_CMD, ICW1_INIT | ICW1_ICW4);
  io_wait();
  outb(PIC_MASTER_DATA, offset0);
  io_wait();
  outb(PIC_SLAVE_DATA, offset1);
  io_wait();
  outb(PIC_MASTER_DATA, 4);
  io_wait();
  outb(PIC_SLAVE_DATA, 2);
  io_wait();

  outb(PIC_MASTER_DATA, ICW4_8086);
  io_wait();
  outb(PIC_SLAVE_DATA, ICW4_8086);
  io_wait();

  outb(PIC_MASTER_DATA, a0);
  outb(PIC_SLAVE_DATA, a1);
}

void pic_disable() {
  outb(PIC_MASTER_DATA, 0xff);
  outb(PIC_SLAVE_DATA, 0xff);
}

void pic_mask(u8 irq) {
  u16 port;
  u8 value;

  if (irq < 8)
    port = PIC_MASTER_DATA;
  else {
    port = PIC_SLAVE_DATA;
    irq -= 8;
  }
  value = inb(port) | (1 << irq);
  outb(port, value);
}

void pic_unmask(u8 irq) {
  u16 port;
  u8 value;

  if (irq < 8)
    port = PIC_MASTER_DATA;
  else {
    port = PIC_SLAVE_DATA;
    irq -= 8;
  }
  value = inb(port) & ~(1 << irq);
  outb(port, value);
}

void pic_send_eoi(u8 irq) {
  if (irq >= 8)
    outb(PIC_SLAVE_CMD, PIC_EOI);
  outb(PIC_MASTER_CMD, PIC_EOI);
}

static u16 _pic_get_irq_reg(int ocw3) {
  outb(PIC_MASTER_CMD, ocw3);
  outb(PIC_SLAVE_CMD, ocw3);
  return (inb(PIC_SLAVE_CMD) << 8) | inb(PIC_MASTER_CMD);
}

u16 pic_get_irr() { return _pic_get_irq_reg(PIC_READ_IRR); }

u16 pic_get_isr() { return _pic_get_irq_reg(PIC_READ_ISR); }
