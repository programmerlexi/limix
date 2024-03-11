#include <io/pio.h>
#include <stdint.h>

void outb(uint16_t port, uint8_t val) {
  asm volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}
void outw(uint16_t port, uint16_t val) {
  asm volatile("outw %w0, %w1" : : "a"(val), "Nd"(port) : "memory");
}
void outd(uint16_t port, uint32_t val) {
  asm volatile("out %0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

uint8_t inb(uint16_t port) {
  uint8_t ret;
  asm volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}
uint16_t inw(uint16_t port) {
  uint8_t ret;
  asm volatile("inw %w1, %w0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}
uint32_t ind(uint16_t port) {
  uint8_t ret;
  asm volatile("in %w1, %0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}
