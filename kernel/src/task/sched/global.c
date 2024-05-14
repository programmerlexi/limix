#include "task/sched/global.h"
#include "asm_inline.h"
#include "debug.h"
#include "kernel.h"
#include "kipc/spinlock.h"
#include "mm/heap.h"
#include "printing.h"
#include "task/proc/proc.h"
#include "task/sched/common.h"
#include "task/sched/local.h"
#include "task/thread/thread.h"
#include "utils/strings/xstr.h"
#include <stdint.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "sched_global"

static process_t *_procs;
static local_scheduler_t *_scheds;
static u32 _glob_sched_lock;
static sched_frame_t *_frames;

void sched_glob_init() {
  log(LOGLEVEL_DEBUG, "Creating main process");
  _procs = proc_create();
  if (!_procs)
    kernel_panic_error("Out of memory");
  _procs->pid = 0;
  _procs->uid = -1;
  _procs->gid = -1;
  _procs->euid = -1;
  _procs->egid = -1;
  _procs->cr4 = read_cr4();
  _procs->thread_count = 1;
  _procs->threads = thread_create();
  if (!_procs->threads)
    kernel_panic_error("Out of memory");
  _procs->name = to_xstr("kernel");
  _procs->cpu = 0;
  log(LOGLEVEL_ANALYZE, "Filling task state");
  thread_switch(_procs->threads, _procs->threads);
  _glob_sched_lock = 0;
  log(LOGLEVEL_INFO, "Global scheduler initialized");
}

void sched_glob_aquire() { spinlock(&_glob_sched_lock); }
void sched_glob_release() { spinunlock(&_glob_sched_lock); }

void sched_glob_tick() {
  sched_glob_aquire();
  local_scheduler_t *c = _scheds;
  while (c) {
    if (!c->shed_lock) {
      spinlock(&c->shed_lock);

      sched_frame_t *cf = _frames;
      while (cf) {
        if (!cf->assigned) {
          if (cf->proc->cpu == c->cpu) {
            frame_container_t *nfc = kmalloc(sizeof(frame_container_t));
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
  ls->next = _scheds;
  _scheds = ls;
  log(LOGLEVEL_DEBUG, "Registered scheduler");

  sched_glob_release();
}

void sched_glob_list_processes() {
  process_t *p = _procs;
  while (p) {
    logf(LOGLEVEL_INFO, "Process '%s' (PID %u) Threads: %u\n\r", p->name.cstr,
         p->pid, p->thread_count);
    p = p->next;
  }
}
