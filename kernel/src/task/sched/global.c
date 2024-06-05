#include "kernel/task/sched/global.h"
#include "kernel/asm_inline.h"
#include "kernel/debug.h"
#include "kernel/kernel.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/mm.h"
#include "kernel/task/proc/proc.h"
#include "kernel/task/sched/common.h"
#include "kernel/task/sched/local.h"
#include "kernel/task/thread/thread.h"
#include "libk/ipc/spinlock.h"
#include "libk/utils/strings/xstr.h"
#include <stdbool.h>
#include <stddef.h>
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
  _procs->cr3 = read_cr3();
  _procs->thread_count = 1;
  _procs->threads = thread_create();
  if (!_procs->threads)
    kernel_panic_error("Out of memory");
  _procs->name = to_xstr("kernel");
  _procs->cpu = -1;
  log(LOGLEVEL_ANALYZE, "Filling task state");
  thread_switch(_procs->threads, _procs->threads);
  _frames = kmalloc(sizeof(sched_frame_t));
  if (!_frames)
    kernel_panic_error("Out of memory");
  _frames->assigned = false;
  _frames->proc = _procs;
  _frames->thread = _procs->threads;
  _glob_sched_lock = 0;
  log(LOGLEVEL_INFO, "Global scheduler initialized");
}

void sched_glob_aquire() { spinlock(&_glob_sched_lock); }
void sched_glob_release() { spinunlock(&_glob_sched_lock); }

void sched_glob_tick() {
  sched_glob_aquire();
  debug("Ticking global");
  local_scheduler_t *c = _scheds;
  while (c) {
    if (!c->shed_lock) {
      debug("Handling scheduler");
      spinlock(&c->shed_lock);

      sched_frame_t *cf = _frames;
      while (cf) {
        debug("Handling frame");
        if (!cf->assigned) {
          if (cf->proc->cpu == c->cpu) {
            debug("Inserting frame");
            frame_container_t *nfc = kmalloc(sizeof(frame_container_t));
            frame_container_t **cfc = &c->frames;
            while (*cfc) {
              cfc = &((*cfc)->next);
            }
            *cfc = nfc;
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

static u64 latest_pid = 1;
void sched_create(void(*start), u64 cpu) {
  sched_glob_aquire();
  process_t *proc = proc_create();
  if (!proc)
    kernel_panic_error("Out of memory");
  proc->pid = latest_pid++;
  proc->uid = -1;
  proc->gid = -1;
  proc->euid = -1;
  proc->egid = -1;
  proc->cr3 = read_cr3();
  proc->thread_count = 1;
  proc->threads = thread_create();
  if (!proc->threads)
    kernel_panic_error("Out of memory");
  proc->name = to_xstr("process");
  proc->cpu = cpu;
  void *rbp = request_page();
  if (!rbp)
    kernel_panic_error("Out of memory");
  proc->threads->rsp = ((u64)rbp + 0x1000) - (8 * 8);
  logf(LOGLEVEL_ANALYZE, "RBP: %x RSP: %x", rbp, proc->threads->rsp);
  ((uint64_t *)(proc->threads->rsp))[7] = (uptr)start;
  ((uint64_t *)(proc->threads->rsp))[6] = 0;
  ((uint64_t *)(proc->threads->rsp))[5] = read_flags();
  ((uint64_t *)(proc->threads->rsp))[4] = 5;
  ((uint64_t *)(proc->threads->rsp))[3] = 4;
  ((uint64_t *)(proc->threads->rsp))[2] = 3;
  ((uint64_t *)(proc->threads->rsp))[1] = 2;
  ((uint64_t *)(proc->threads->rsp))[0] = 1;
  proc->next = _procs->next;
  _procs->next = proc;
  sched_frame_t *frame = kmalloc(sizeof(*frame));
  if (!frame)
    kernel_panic_error("Out of memory");
  frame->proc = proc;
  frame->thread = proc->threads;
  frame->assigned = false;
  frame->next = _frames->next;
  _frames->next = frame;
  sched_glob_release();
}

void sched_register_cpu(local_scheduler_t *ls) {
  sched_glob_aquire();

  log(LOGLEVEL_ANALYZE, "Registering scheduler");

  log(LOGLEVEL_ANALYZE, "Creating core process");
  process_t *proc = proc_create();
  if (!proc)
    kernel_panic_error("Out of memory");
  log(LOGLEVEL_ANALYZE, "Filling process info");
  proc->pid = 0;
  proc->uid = -1;
  proc->gid = -1;
  proc->euid = -1;
  proc->egid = -1;
  log(LOGLEVEL_ANALYZE, "Reading CR3");
  proc->cr3 = read_cr3();
  log(LOGLEVEL_ANALYZE, "Doing threads");
  proc->thread_count = 1;
  proc->threads = thread_create();
  if (!proc->threads)
    kernel_panic_error("Out of memory");
  log(LOGLEVEL_ANALYZE, "Attach name and cpu");
  proc->name = to_xstr("kernel_core");
  proc->cpu = ls->cpu;
  log(LOGLEVEL_ANALYZE, "Creating frame");
  sched_frame_t *frame = kmalloc(sizeof(sched_frame_t));
  if (!frame)
    kernel_panic_error("Out of memory");
  frame->assigned = true;
  frame->proc = proc;
  frame->thread = proc->threads;
  log(LOGLEVEL_ANALYZE, "Inserting process");
  proc->next = _procs->next;
  log(LOGLEVEL_ANALYZE, "Inserting process (Stage 2)");
  _procs->next = proc;
  ls->frames = NULL;
  ls->core_process = proc;

  ls->next = _scheds;
  _scheds = ls;
  log(LOGLEVEL_DEBUG, "Registered scheduler");

  sched_glob_release();
  sched_glob_tick();
}

void sched_glob_list_processes() {
  process_t *p = _procs;
  while (p) {
    logf(LOGLEVEL_INFO, "Process '%s' (PID %u) Threads: %u", p->name.cstr,
         p->pid, p->thread_count);
    p = p->next;
  }
}

local_scheduler_t *sched_get_local(u64 cpu) {
  sched_glob_aquire();
  local_scheduler_t *l = _scheds;
  while (l) {
    if (l->cpu == cpu) {
      sched_glob_release();
      return l;
    }
    l = l->next;
  }
  sched_glob_release();
  return NULL;
}
