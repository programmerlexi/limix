#define DEBUG_MODULE "sched_global"

#include <asm_inline.h>
#include <debug.h>
#include <kernel.h>
#include <kipc/spinlock.h>
#include <mm/heap.h>
#include <printing.h>
#include <stdint.h>
#include <task/proc/proc.h>
#include <task/sched/common.h>
#include <task/sched/global.h>
#include <task/sched/local.h>
#include <task/thread/thread.h>
#include <utils/strings/xstr.h>

static process_t *procs;
static local_scheduler_t *scheds;
static uint32_t glob_sched_lock;
static sched_frame_t *frames;

void sched_glob_init() {
  log(LOGLEVEL_DEBUG, "Creating main process");
  procs = proc_create();
  if (!procs)
    kernel_panic_error("Out of memory");
  procs->pid = 0;
  procs->uid = -1;
  procs->gid = -1;
  procs->euid = -1;
  procs->egid = -1;
  procs->cr4 = read_cr4();
  procs->thread_count = 1;
  procs->threads = thread_create();
  if (!procs->threads)
    kernel_panic_error("Out of memory");
  procs->name = to_xstr("kernel");
  procs->cpu = 0;
  log(LOGLEVEL_ANALYZE, "Filling task state");
  thread_switch(procs->threads, procs->threads);
  glob_sched_lock = 0;
  log(LOGLEVEL_INFO, "Global scheduler initialized");
}

void sched_glob_aquire() { spinlock(&glob_sched_lock); }
void sched_glob_release() { spinunlock(&glob_sched_lock); }

void sched_glob_tick() {
  sched_glob_aquire();
  local_scheduler_t *c = scheds;
  while (c) {
    if (!c->shed_lock) {
      spinlock(&c->shed_lock);

      sched_frame_t *cf = frames;
      while (cf) {
        if (!cf->assigned) {
          if (cf->proc->cpu == c->cpu) {
            frame_container_t *nfc = malloc(sizeof(frame_container_t));
            nfc->next = c->frames;
            c->frames = nfc;
            nfc->frame = cf;
            cf->assigned = true;
          }
        }
        cf = cf->next;
      }

      spinunlock(&c->shed_lock);
    }
    c = c->next;
  }
  sched_glob_release();
}

void sched_register_cpu(local_scheduler_t *ls) {
  sched_glob_aquire();

  log(LOGLEVEL_ANALYZE, "Registering scheduler");
  ls->next = scheds;
  scheds = ls;
  log(LOGLEVEL_DEBUG, "Registered scheduler");

  sched_glob_release();
}

void sched_glob_list_processes() {
  process_t *p = procs;
  while (p) {
    kprintf("Process '%s' (PID %u) Threads: %u\n\r", p->name.cstr, (int)p->pid,
            (int)p->thread_count);
    p = p->next;
  }
}
