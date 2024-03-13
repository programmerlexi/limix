#include <boot/limine.h>
#include <config.h>
#include <gfx/framebuffer.h>
#include <gfx/vga.h>
#include <io/serial/serial.h>
#include <kipc/semaphore.h>
#include <mm/hhtp.h>
#include <mm/mm.h>
#include <smp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <task/async.h>

static volatile LIMINE_BASE_REVISION(1);

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0, .response = NULL};

static volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST, .revision = 0, .response = NULL};

static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST, .revision = 0, .response = NULL};

static volatile struct limine_paging_mode_request paging_request = {
    .id = LIMINE_PAGING_MODE_REQUEST,
    .revision = 0,
    .mode = USED_PAGING_MODE,
    .response = NULL};

static volatile struct limine_memmap_request mmap_request = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = NULL};

void hcf(void) {
  asm("cli");
  for (;;) {
    asm("hlt");
  }
}

uint64_t hhaddr;
semaphore_t smp_cpu = 0;

void _smp_start(struct limine_smp_info *cpu_info) {
  putstr16((cpu_info->processor_id * 9), 0, "P", 0xff0000);

  smp_cmd_t *cmd = (smp_cmd_t *)&cpu_info->extra_argument;
  cmd->data = 0;
  cmd->state = SMP_STATE_INIT_CMD;

  unblock(&smp_cpu);

  putstr16((cpu_info->processor_id * 9), 0, "P", 0xffff00);
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
    putstr16((cpu_info->processor_id * 9), 0, "P", 0x0000ff);
    cmd->state = SMP_STATE_FROZEN;
    hcf();
  }

  cmd->cmd_type = SMP_ACTION_NONE;
  cmd->state = SMP_STATE_INIT_WAIT_ACK;

  while (cmd->cmd_type != SMP_ACTION_ACK) {
    asm volatile("nop");
  }
  putchar16((cpu_info->processor_id * 9), 0, 'P', 0x00ff00);
  cmd->cmd_type = SMP_ACTION_NONE;
  cmd->state = SMP_STATE_READY;

  while (true) {
    if (cmd->cmd_type == SMP_ACTION_NONE)
      continue;
    switch (cmd->cmd_type) {}
    cmd->state = SMP_STATE_BUSY;
  }
}

void _start(void) {
  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    hcf();
  }

  serial_early_init();
  serial_writes("If this is visible serial was successful\n\r");

  if (framebuffer_request.response == NULL ||
      framebuffer_request.response->framebuffer_count < 1) {
    serial_writes("[!!] This kernel is meant for graphical systems. Please "
                  "attach a monitor!\n\r");
    hcf();
  }

  init_fb(framebuffer_request.response->framebuffers[0]);

  if (paging_request.response->mode != USED_PAGING_MODE) {
    putstr16(0, 0, "Paging mode doesn't match! ABORTING!", 0xff0000);
    serial_writes("[!!] Paging mode isn't matching!\n\r");
    hcf();
  }

  hhaddr = hhdm_request.response->offset;

  if (!mm_init(mmap_request.response)) {
    putstr16(0, 0, "MM init failed! ABORTING!", 0xff0000);
    serial_writes("[!!] MM seems to have failed\n\r");
    hcf();
  }

  async_init();

  serial_writes("Performing SMP initialization!\n\r");

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
        putstr16(0, i * 17, "Timed out", 0xff0000);
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
        putstr16(0, i * 17, "Timed out", 0xff0000);
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
        putstr16(0, i * 17, "Timed out", 0xff0000);
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
        putstr16(0, i * 17, "Timed out", 0xff0000);
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
        putstr16(0, i * 17, "Timed out", 0xff0000);
        break;
      }
    }

    if (max_cycles <= 0)
      continue;

    if (cmd->state == SMP_STATE_FROZEN) {
      putstr16(0, i * 17, "Processor froze", 0xff0000);
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
      putstr16(0, i * 17, "Processor done", 0xffffff);
    } else {
      putstr16(0, i * 17, "Processor produced garbage", 0xffff00);
    }
  }

  putchar16(0, 0, 'P', 0x00ff00);

  hcf();
}
