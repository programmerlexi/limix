#pragma once

#include <stdbool.h>
#include <stdint.h>

static inline bool are_interrupts_enabled() {
  unsigned long flags;
  asm volatile("pushf\n\t"
               "pop %0"
               : "=g"(flags));
  return flags & (1 << 9);
}

static inline unsigned long save_irqdisable(void) {
  unsigned long flags;
  asm volatile("pushf\n\tcli\n\tpop %0" : "=r"(flags) : : "memory");
  return flags;
}

static inline void irqrestore(unsigned long flags) {
  asm("push %0\n\tpopf" : : "rm"(flags) : "memory", "cc");
}

static inline void cpuid(int code, uint32_t *a, uint32_t *d) {
  asm volatile("cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx");
}

static inline uint64_t rdtsc(void) {
  uint32_t low, high;
  asm volatile("rdtsc" : "=a"(low), "=d"(high));
  return ((uint64_t)high << 32) | low;
}

static inline unsigned long read_cr0(void) {
  unsigned long val;
  asm volatile("mov %%cr0, %0" : "=r"(val));
  return val;
}
static inline unsigned long read_cr1(void) {
  unsigned long val;
  asm volatile("mov %%cr1, %0" : "=r"(val));
  return val;
}
static inline unsigned long read_cr2(void) {
  unsigned long val;
  asm volatile("mov %%cr2, %0" : "=r"(val));
  return val;
}
static inline unsigned long read_cr3(void) {
  unsigned long val;
  asm volatile("mov %%cr3, %0" : "=r"(val));
  return val;
}
static inline unsigned long read_cr4(void) {
  unsigned long val;
  asm volatile("mov %%cr4, %0" : "=r"(val));
  return val;
}

static inline void write_cr4(uint64_t cr4) {
  asm volatile("mov %0, %%cr4" ::"r"(cr4));
}

static inline void invlpg(void *m) {
  /* Clobber memory to avoid optimizer re-ordering access before invlpg, which
   * may cause nasty bugs. */
  asm volatile("invlpg (%0)" : : "b"(m) : "memory");
}

static inline void wrmsr(uint64_t msr, uint64_t value) {
  uint32_t low = value & 0xFFFFFFFF;
  uint32_t high = value >> 32;
  asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

static inline uint64_t rdmsr(uint64_t msr) {
  uint32_t low, high;
  asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
  return ((uint64_t)high << 32) | low;
}

#define fence() __asm__ volatile("" ::: "memory")
