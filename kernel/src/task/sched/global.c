#define DEBUG_MODULE "sched_global"

#include <asm_inline.h>
#include <debug.h>
#include <kipc/spinlock.h>
#include <mm/heap.h>
#include <stdint.h>
#include <task/proc/proc.h>
#include <task/sched/common.h>
#include <task/sched/global.h>
#include <task/sched/local.h>
#include <task/thread/thread.h>
#include <utils/strings/xstr.h>

process_t *procs;
local_scheduler_t *scheds;
uint32_t glob_sched_lock;
sched_frame_t *frames;

void sched_glob_init() {
  debug("Creating 'main_kernel' task");
  procs = proc_create();
  procs->pid = 0;
  procs->uid = -1;
  procs->gid = -1;
  procs->euid = -1;
  procs->egid = -1;
  procs->cr4 = read_cr4();
  procs->thread_count = 1;
  procs->threads = thread_create();
  procs->name = to_xstr("main_kernel");
  procs->cpu = 0;
  debug("Filling task state");
  thread_switch(procs->threads, procs->threads);
  glob_sched_lock = 0;
  info("Global scheduler initialized");
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

  debug("Registering scheduler");
  ls->next = scheds;
  scheds = ls;
  debug("Registered scheduler");

  sched_glob_release();
}
