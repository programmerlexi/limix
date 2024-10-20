#include <kernel/debug.h>
#include <kernel/hw/ps2.h>
#include <kernel/io/pio.h>
#include <stdbool.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "ps/2"

static bool _dual_channel = true;
static bool _port1_available = false;
static bool _port2_available = false;

void ps2_send_command1(u8 cmd) { ps2_send_data1(cmd); }
void ps2_send_data1(u8 data) {
  if (!_port1_available)
    return;
  if (!ps2_can_send())
    return;
  outb(PS2_PORT_DATA, data);
}

void ps2_send_command2(u8 cmd) { ps2_send_data2(cmd); }
void ps2_send_data2(u8 data) {
  if (!_port2_available)
    return;
  if (!ps2_can_send())
    return;
  ps2_send_command(PS2_COMMAND_WRITE_PORT2_INPUT);
  while (!ps2_can_send())
    asm("nop");
  outb(PS2_PORT_DATA, data);
}

u8 ps2_read_data2() {
  if (!_port2_available)
    return 0;
  if (!ps2_data_available())
    return 0;
  return ps2_read_data();
}

u8 ps2_read_data() {
  if (ps2_data_available())
    return inb(PS2_PORT_DATA);
  return 0;
}
bool ps2_data_available() {
  return inb(PS2_PORT_STATUS) & PS2_STATUS_OUTPUT_BUFFER_FULL;
}

void ps2_send_command(u8 command) { outb(PS2_PORT_COMMAND, command); }

void ps2_send_data(u8 data) {
  if (ps2_can_send())
    outb(PS2_PORT_DATA, data);
}
bool ps2_can_send() {
  return !(inb(PS2_PORT_STATUS) & PS2_STATUS_INPUT_BUFFER_FULL);
}

void ps2_reset_cpu();

void ps2_init() {
  log(LOGLEVEL_ANALYZE, "Disabling port 1");
  ps2_send_command(PS2_COMMAND_DISABLE_PORT1);

  log(LOGLEVEL_ANALYZE, "Disabling port 2");
  ps2_send_command(PS2_COMMAND_DISABLE_PORT2);

  log(LOGLEVEL_ANALYZE, "Clearing controller data");
  while (ps2_data_available())
    ps2_read_data();

  log(LOGLEVEL_ANALYZE, "Getting CCB");
  ps2_send_command(PS2_COMMAND_READ_CCB);
  while (!ps2_data_available())
    asm("nop");
  u8 ccb = ps2_read_data();
  _dual_channel = ccb & PS2_CCB_PORT2_CLOCK;
  ps2_send_command(PS2_COMMAND_WRITE_CCB);
  while (!ps2_can_send())
    asm("nop");

  log(LOGLEVEL_ANALYZE, "Disabling Interrupts + Scancode translation");
  ps2_send_data(ccb & ~(PS2_CCB_PORT1_INT | PS2_CCB_PORT2_INT |
                        PS2_CCB_PORT1_TRANSLATION));

  log(LOGLEVEL_ANALYZE, "Performing controller self-test");
  ps2_send_command(PS2_COMMAND_TEST_CONTROLLER);
  while (!ps2_data_available())
    asm("nop");
  u8 result = ps2_read_data();
  if (result != 0x55) {
    log(LOGLEVEL_CRITICAL, "PS/2 Controller failed self-test");
    return;
  }
  ps2_send_command(PS2_COMMAND_WRITE_CCB);
  while (!ps2_can_send())
    asm("nop");
  ps2_send_data(ccb & ~(PS2_CCB_PORT1_INT | PS2_CCB_PORT2_INT |
                        PS2_CCB_PORT1_TRANSLATION));
  if (_dual_channel) {
    log(LOGLEVEL_ANALYZE, "Checking dual-channel");
    ps2_send_command(PS2_COMMAND_ENABLE_PORT2);
    io_wait();
    u8 ccb = ps2_read_data();
    _dual_channel = !(ccb & PS2_CCB_PORT2_CLOCK);
    if (_dual_channel)
      ps2_send_command(PS2_COMMAND_DISABLE_PORT2);
  }
  log(LOGLEVEL_ANALYZE, "Testing port 1");
  ps2_send_command(PS2_COMMAND_TEST_PORT1);
  while (!ps2_data_available())
    asm("nop");
  if (ps2_read_data()) {
    log(LOGLEVEL_WARN1, "WARNING: First PS/2 port failed, keyboard may "
                        "not be available.");
  } else {
    _port1_available = true;
  }
  if (_dual_channel) {
    log(LOGLEVEL_ANALYZE, "Testing port 2");
    ps2_send_command(PS2_COMMAND_TEST_PORT2);
    while (!ps2_data_available())
      asm("nop");
    if (ps2_read_data()) {
      log(LOGLEVEL_WARN1, "WARNING: Second PS/2 port failed, mouse may "
                          "not be available.");
    } else {
      _port2_available = true;
    }
  }
  if (!(_port1_available || _port2_available)) {
    log(LOGLEVEL_CRITICAL,
        "ERROR: All PS/2 ports failed, keyboard and mouse will not be "
        "available using PS/2.");
    return;
  }
  log(LOGLEVEL_INFO, "Finished initialization");
}
