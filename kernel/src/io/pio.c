#include <io/pio.h>
#include <stdint.h>

void outb(u16 port, u8 val) {
  asm volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}
void outw(u16 port, u16 val) {
  asm volatile("outw %w0, %w1" : : "a"(val), "Nd"(port) : "memory");
}
void outd(u16 port, u32 val) {
  asm volatile("outl %0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

u8 inb(u16 port) {
  u8 ret;
  asm volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}
u16 inw(u16 port) {
  u8 ret;
  asm volatile("inw %w1, %w0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}

u32 ind(u16 port) {
  u32 returnVal;
  asm volatile("inl %1, %0" : "=a"(returnVal) : "Nd"(port));
  return returnVal;
}
