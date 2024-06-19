#include "kernel/hw/pit/pit.h"
#include "kernel/hw/cpu/irq.h"
#include "kernel/hw/cpu/pic.h"
#include "kernel/int/syscall.h"
#include "kernel/io/pio.h"

static void pit_handler() {
  pic_send_eoi(0);
  call_syscall(SYSCALL_YIELD);
}

void pit_init() {
  irq_register(pit_handler, 0);
  pit_set_freq(100);
  pic_unmask(0);
}

void pit_set_freq(u64 freq) { pit_set_divisor(PIT_BASE_FREQ / freq); }

void pit_set_divisor(u64 divisor) {
  if (divisor < 100)
    divisor = 100;
  outb(PIT_CHANNEL0, (u8)(divisor & 0xff));
  io_wait();
  outb(PIT_CHANNEL0, (u8)((divisor & 0xff00) >> 8));
}
