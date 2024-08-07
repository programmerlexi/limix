#pragma once
#include "libk/types.h"
#include "limine.h"

/* SMP actions (CMDs) */
#define SMP_ACTION_NONE 0
#define SMP_ACTION_DATA 1
#define SMP_ACTION_ACK 2

/* SMP CMD states */
#define SMP_STATE_READY 0
#define SMP_STATE_BUSY 1
#define SMP_STATE_DONE 2

/* SMP INIT states */
#define SMP_STATE_INIT_CMD 3
#define SMP_STATE_INIT_CMD2 4
#define SMP_STATE_INIT_LAPIC 5
#define SMP_STATE_INIT_PID 6
#define SMP_STATE_INIT_WAIT_ACK 7

/* SMP frozen state */
#define SMP_STATE_FROZEN 0xff

typedef union {
  struct {
    u8 state;
    u8 cmd_type;
    u8 cmd_args[6];
  };
  u64 data;
} SmpCommand;

void _smp_start(struct limine_smp_info *info_structure);
u64 smp_init();
