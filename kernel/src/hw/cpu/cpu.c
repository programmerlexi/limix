#include <kernel/asm_inline.h>
#include <kernel/debug.h>
#include <kernel/hw/cpu/cpu.h>
#include <kernel/hw/cpu/cpuid.h>
#include <kernel/mm/heap.h>
#include <libk/crypt/rand.h>
#include <libk/ipc/spinlock.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "cpu"

static CpuListEntry *cpu_list;
static u32 cpu_list_lock;
static u64 cpus;

void cpu_init() {
  spinlock(&cpu_list_lock);

  CpuListEntry *n = kmalloc(sizeof(*n));
  n->next = cpu_list;
  n->cpu_id = get_processor();

  u32 a, b, c, _;
  cpuid(0, &_, &a, &c, &b);

  for (u8 i = 0; i < 4; i++)
    n->vendor[0 + i] = (a >> (8 * i)) & 0xff;
  for (u8 i = 0; i < 4; i++)
    n->vendor[4 + i] = (b >> (8 * i)) & 0xff;
  for (u8 i = 0; i < 4; i++)
    n->vendor[8 + i] = (c >> (8 * i)) & 0xff;
  n->vendor[12] = 0;

  cpuid(1, &_, &_, &b, &a);

  n->capabilities = 0;

  if (b & CPUID_FEAT_ECX_RDRAND)
    n->capabilities |= CPU_FEAT_RDRAND;
  if (b & CPUID_FEAT_ECX_X2APIC)
    n->capabilities |= CPU_FEAT_X2APIC;
  if (a & CPUID_FEAT_EDX_APIC)
    n->capabilities |= CPU_FEAT_APIC;

  cpu_list = n;
  cpus++;

  spinunlock(&cpu_list_lock);
  logf(LOGLEVEL_DEBUG, "[CPU %i] Created cpu info", n->cpu_id);
}

bool cpu_has(CpuCapability capability) {
  u64 cpu_id = get_processor();
  spinlock(&cpu_list_lock);

  CpuListEntry *e = cpu_list;

  while (e) {
    if (e->cpu_id == cpu_id) {
      spinunlock(&cpu_list_lock);
      return e->capabilities & capability;
    }
    e = e->next;
  }

  spinunlock(&cpu_list_lock);
  return false;
}

char *cpu_vendor() {
  u64 cpu_id = get_processor();
  spinlock(&cpu_list_lock);

  CpuListEntry *e = cpu_list;

  while (e) {
    if (e->cpu_id == cpu_id) {
      spinunlock(&cpu_list_lock);
      return e->vendor;
    }
    e = e->next;
  }

  spinunlock(&cpu_list_lock);
  return "None";
}

i64 cpu_get_random_cpu() {
  spinlock(&cpu_list_lock);

  i64 c = krand() % cpus;
  CpuListEntry *cs = cpu_list;
  for (i64 i = 0; i < c; i++) {
    cs = cs->next;
  }

  spinunlock(&cpu_list_lock);

  logf(LOGLEVEL_DEBUG, "Random cpu: %i", cs->cpu_id);

  return cs->cpu_id;
}
