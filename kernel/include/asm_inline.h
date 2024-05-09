#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <types.h>

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

static inline void cpuid(i32 code, u32 *a, u32 *d) {
  asm volatile("cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx");
}

static inline u64 rdtsc(void) {
  u32 low, high;
  asm volatile("rdtsc" : "=a"(low), "=d"(high));
  return ((u64)high << 32) | low;
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

static inline void write_cr4(u64 cr4) {
  asm volatile("mov %0, %%cr4" ::"r"(cr4));
}

static inline void invlpg(void *m) {
  /* Clobber memory to avoid optimizer re-ordering access before invlpg, which
   * may cause nasty bugs. */
  asm volatile("invlpg (%0)" : : "b"(m) : "memory");
}

static inline void wrmsr(u64 msr, u64 value) {
  u32 low = value & 0xFFFFFFFF;
  u32 high = value >> 32;
  asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

static inline u64 rdmsr(u64 msr) {
  u32 low, high;
  asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
  return ((u64)high << 32) | low;
}

#define fence() __asm__ volatile("" ::: "memory")
