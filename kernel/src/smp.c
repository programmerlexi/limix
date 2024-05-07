#define DEBUG_MODULE "smp"

#include <boot/limine.h>
#include <boot/requests.h>
#include <debug.h>
#include <gfx/framebuffer.h>
#include <kernel.h>
#include <kipc/semaphore.h>
#include <smp.h>
#include <stdbool.h>
#include <task/sched/local.h>

static semaphore_t smp_cpu = 0;
void unlock_lschedi();

void _smp_start(struct limine_smp_info *cpu_info) {
  // putstr16((cpu_info->processor_id * 9), 0, "P", 0xff0000);

  smp_cmd_t *cmd = (smp_cmd_t *)&cpu_info->extra_argument;
  cmd->data = 0;
  cmd->state = SMP_STATE_INIT_CMD;

  unblock(&smp_cpu);

  // putstr16((cpu_info->processor_id * 9), 0, "P", 0xffff00);
  {
    while (cmd->cmd_type != SMP_ACTION_DATA) {
      asm volatile("nop");
    }
    uint64_t new_cmd = cmd->data & 0x0000ffffffffffff;
    cmd->cmd_type = SMP_ACTION_NONE;
    cmd->state = SMP_STATE_INIT_CMD2;

    while (cmd->cmd_type != SMP_ACTION_DATA) {
      asm volatile("nop");
    }
    new_cmd = new_cmd | (cmd->data & 0x0000ffff00000000) << 16;

    cmd->state = SMP_STATE_DONE;
    /*if (new_cmd != 0)
      cmd = (smp_cmd_t *)new_cmd;*/ // No idea why this won't work
  }

  cmd->cmd_type = SMP_ACTION_NONE;
  cmd->state = SMP_STATE_INIT_LAPIC;

  while (cmd->cmd_type != SMP_ACTION_DATA) {
    asm volatile("nop");
  }
  uint32_t lapic_id = (cmd->data & 0x0000ffffffff0000) >> 16;
  cmd->cmd_type = SMP_ACTION_NONE;
  cmd->state = SMP_STATE_INIT_PID;

  while (cmd->cmd_type != SMP_ACTION_DATA) {
    asm volatile("nop");
  }
  uint32_t processor_id = (cmd->data & 0x0000ffffffff0000) >> 16;

  // Validate data
  if (cpu_info->lapic_id != lapic_id ||
      cpu_info->processor_id != processor_id) {
    // putstr16((cpu_info->processor_id * 9), 0, "P", 0x0000ff);
    cmd->state = SMP_STATE_FROZEN;
    hcf();
  }

  cmd->cmd_type = SMP_ACTION_NONE;
  cmd->state = SMP_STATE_INIT_WAIT_ACK;

  while (cmd->cmd_type != SMP_ACTION_ACK) {
    asm volatile("nop");
  }
  // putchar16((cpu_info->processor_id * 9), 0, 'P', 0x00ff00);
  cmd->cmd_type = SMP_ACTION_NONE;
  cmd->state = SMP_STATE_READY;

  local_scheduler_t *ls = sched_local_init(processor_id);

  while (true) {
    sched_local_tick(ls);
  }
}

void smp_init() {
  for (uint64_t i = 0; i < smp_request.response->cpu_count; i++) {
    if (smp_request.response->bsp_lapic_id ==
        smp_request.response->cpus[i]->lapic_id)
      continue;
    smp_request.response->cpus[i]->goto_address = _smp_start;
  }
  block_on_count(&smp_cpu, smp_request.response->cpu_count - 1);

  for (uint64_t i = 0; i < smp_request.response->cpu_count; i++) {
    if (smp_request.response->bsp_lapic_id ==
        smp_request.response->cpus[i]->lapic_id)
      continue;
    smp_cmd_t *cmd =
        (smp_cmd_t *)&(smp_request.response->cpus[i]->extra_argument);

    int64_t max_cycles = 0x1ffffff;

    while (cmd->state != SMP_STATE_INIT_CMD) {
      asm volatile("nop");
      max_cycles--;
      if (max_cycles <= 0) {
        // putstr16(0, i * 17, "Timed out", 0xff0000);
        break;
      }
    }

    if (max_cycles <= 0)
      continue;

    cmd->data = (cmd->data & 0xffff000000000000) |
                (((uint64_t)cmd & 0x0000ffffffffffff));
    cmd->cmd_type = SMP_ACTION_DATA;

    while (cmd->state != SMP_STATE_INIT_CMD2) {
      asm volatile("nop");
      max_cycles--;
      if (max_cycles <= 0) {
        // putstr16(0, i * 17, "Timed out", 0xff0000);
        break;
      }
    }

    if (max_cycles <= 0)
      continue;

    cmd->data = (cmd->data & 0xffff0000ffffffff) |
                (((uint64_t)cmd & 0xffff000000000000) >> 16);
    cmd->cmd_type = SMP_ACTION_DATA;

    while (cmd->state != SMP_STATE_INIT_LAPIC) {
      asm volatile("nop");
      max_cycles--;
      if (max_cycles <= 0) {
        // putstr16(0, i * 17, "Timed out", 0xff0000);
        break;
      }
    }

    if (max_cycles <= 0)
      continue;

    cmd->data = smp_request.response->cpus[i]->lapic_id << 16;
    cmd->cmd_type = SMP_ACTION_DATA;

    while (cmd->state != SMP_STATE_INIT_PID) {
      asm volatile("nop");
      max_cycles--;
      if (max_cycles <= 0) {
        // putstr16(0, i * 17, "Timed out", 0xff0000);
        break;
      }
    }

    if (max_cycles <= 0)
      continue;

    cmd->data = smp_request.response->cpus[i]->processor_id << 16;
    cmd->cmd_type = SMP_ACTION_DATA;

    while (cmd->state != SMP_STATE_INIT_WAIT_ACK &&
           cmd->state != SMP_STATE_FROZEN) {
      asm volatile("nop");
      max_cycles--;
      if (max_cycles <= 0) {
        // putstr16(0, i * 17, "Timed out", 0xff0000);
        break;
      }
    }

    if (max_cycles <= 0)
      continue;

    if (cmd->state == SMP_STATE_FROZEN) {
      // putstr16(0, i * 17, "Processor froze", 0xff0000);
      continue;
    } else if (cmd->state == SMP_STATE_INIT_WAIT_ACK) {
      cmd->cmd_type = SMP_ACTION_ACK;
      /*while (cmd->state != SMP_STATE_READY) {
          asm volatile("nop");
          max_cycles--;
          if (max_cycles <= 0) {
              putstr16(0, i * 17, "Timed out", 0xff0000);
              break;
          }
      }

      if (max_cycles <= 0) continue;*/
      // putstr16(0, i * 17, "Processor done", 0xffffff);
    } else {
      // putstr16(0, i * 17, "Processor produced garbage", 0xffff00);
    }
    debug("Processor up");
  }

  debug("All processors up");
  unlock_lschedi();

  // putchar16(0, 0, 'P', 0x00ff00);
}
