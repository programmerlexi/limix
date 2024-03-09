#include "config.h"
#include "hhtp.h"
#include "smp.h"
#include <framebuffer.h>
#include <kipc.h>
#include <limine.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vga.h>

static volatile LIMINE_BASE_REVISION(1);

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

static volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST, .revision = 0};

static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST, .revision = 0};

static volatile struct limine_paging_mode_request paging_request = {
    .id = LIMINE_PAGING_MODE_REQUEST, .revision = 0, .mode = USED_PAGING_MODE};

static volatile struct limine_memmap_request mmap_request = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0};

static volatile struct limine_module_request mod_request = {
    .id = LIMINE_MODULE_REQUEST, .revision = 0};

static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST, .revision = 0};

static volatile struct limine_boot_time_request boot_time_request = {
    .id = LIMINE_BOOT_TIME_REQUEST, .revision = 0};

static volatile struct limine_kernel_address_request kernel_addr_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0};

static volatile struct limine_kernel_file_request kernel_file_request = {
    .id = LIMINE_KERNEL_FILE_REQUEST, .revision = 0};

static void hcf(void) {
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
    new_cmd |= (cmd->data & 0x0000ffff00000000) << 16;

    cmd->state = SMP_STATE_DONE;
    // if (new_cmd != 0) cmd = (smp_cmd_t*) new_cmd;
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

  if (framebuffer_request.response == NULL ||
      framebuffer_request.response->framebuffer_count < 1) {
    hcf();
  }

  init_fb(framebuffer_request.response->framebuffers[0]);

  if (paging_request.response->mode != USED_PAGING_MODE) {
    putstr16(0, 0, "Paging mode doesn't match! ABORTING!", 0xff0000);
    hcf();
  }

  hhaddr = hhdm_request.response->offset;

  for (uint64_t i = 0; i < smp_request.response->cpu_count; i++) {
    if (smp_request.response->bsp_lapic_id ==
        smp_request.response->cpus[i]->lapic_id)
      continue;
    smp_request.response->cpus[i]->goto_address = _smp_start;
    block_on(&smp_cpu);
  }

  for (uint64_t i = 0; i < smp_request.response->cpu_count; i++) {
    if (smp_request.response->bsp_lapic_id ==
        smp_request.response->cpus[i]->lapic_id)
      continue;
    smp_cmd_t *cmd =
        (smp_cmd_t *)&(smp_request.response->cpus[i]->extra_argument);

    int64_t max_cycles = 0xffffff;

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

    cmd->data = 0;
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

    cmd->data = 0;
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
