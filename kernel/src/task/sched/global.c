#include "kernel/task/sched/global.h"
#include "kernel/asm_inline.h"
#include "kernel/debug.h"
#include "kernel/int/syscall.h"
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
static u32 _glob_sched_lock = 1;
static frame_queue_t _frames;

extern void thread_enter();

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
  _frames.start = _frames.end = kmalloc(sizeof(sched_frame_t));
  if (!_frames.start)
    kernel_panic_error("Out of memory");
  _frames.start->assigned = false;
  _frames.start->proc = _procs;
  _frames.start->thread = _procs->threads;
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

      sched_frame_t *cf = _frames.start;
      while (cf) {
        debug("Handling frame");
        if (!cf->assigned) {
          if (cf->proc->cpu == c->cpu) {
            debug("Inserting frame");
            frame_container_t *nfc = kmalloc(sizeof(frame_container_t));
            if (c->frames.end) {
              c->frames.end->next = nfc;
              c->frames.end = nfc;
            } else {
              c->frames.start = nfc;
              c->frames.end = nfc;
            }

            nfc->frame = cf;
            nfc->next = NULL;
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

static void _sched_thread_end() {
  for (;;)
    call_syscall(SYSCALL_YIELD);
}

static u64 latest_pid = 1;
void sched_create(void(*start), i64 cpu, u64 arg) {
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
  proc->threads->rsp = ((u64)rbp + 0x1000) - (8 * 11);
  logf(LOGLEVEL_ANALYZE, "RBP: %x RSP: %x", rbp, proc->threads->rsp);
  ((uint64_t *)(proc->threads->rsp))[10] = (uptr)_sched_thread_end;
  ((uint64_t *)(proc->threads->rsp))[9] = (uptr)start;
  ((uint64_t *)(proc->threads->rsp))[8] = arg;
  ((uint64_t *)(proc->threads->rsp))[7] = (uptr)thread_enter;
  ((uint64_t *)(proc->threads->rsp))[6] = 0;
  ((uint64_t *)(proc->threads->rsp))[5] = read_flags();
  ((uint64_t *)(proc->threads->rsp))[4] = 0;
  ((uint64_t *)(proc->threads->rsp))[3] = 0;
  ((uint64_t *)(proc->threads->rsp))[2] = 0;
  ((uint64_t *)(proc->threads->rsp))[1] = 0;
  ((uint64_t *)(proc->threads->rsp))[0] = 0;
  proc->next = _procs->next;
  proc->prev = _procs;
  if (proc->next)
    proc->next->prev = proc;
  _procs->next = proc;
  sched_frame_t *frame = kmalloc(sizeof(*frame));
  if (!frame)
    kernel_panic_error("Out of memory");
  frame->proc = proc;
  frame->thread = proc->threads;
  frame->assigned = false;
  frame->next = NULL;
  _frames.end->next = frame;
  _frames.end = frame;
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
  if (proc->next)
    proc->next->prev = proc;
  proc->prev = _procs;
  _procs->next = proc;
  ls->frames.start = NULL;
  ls->frames.end = NULL;
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
