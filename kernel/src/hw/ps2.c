#define DEBUG_MODULE "ps/2"
#include <debug.h>
#include <gfx/vt/vt.h>
#include <hw/ps2.h>
#include <io/pio.h>
#include <kernel.h>

static bool dual_channel = true;
static bool port1_available = false;
static bool port2_available = false;

void ps2_send_command1(uint8_t cmd) { ps2_send_data1(cmd); }
void ps2_send_data1(uint8_t data) {
  if (!port1_available)
    return;
  if (!ps2_can_send())
    return;
  outb(PS2_PORT_DATA, data);
}

void ps2_send_command2(uint8_t cmd) { ps2_send_data2(cmd); }
void ps2_send_data2(uint8_t data) {
  if (!port2_available)
    return;
  if (!ps2_can_send())
    return;
  ps2_send_command(PS2_COMMAND_WRITE_PORT2_INPUT);
  while (!ps2_can_send())
    asm("nop");
  outb(PS2_PORT_DATA, data);
}

uint8_t ps2_read_data2() {
  if (!port2_available)
    return 0;
  if (!ps2_data_available())
    return 0;
  return ps2_read_data();
}

uint8_t ps2_read_data() {
  if (ps2_data_available())
    return inb(PS2_PORT_DATA);
  return 0;
}
bool ps2_data_available() {
  return inb(PS2_PORT_STATUS) & PS2_STATUS_OUTPUT_BUFFER_FULL;
}

void ps2_send_command(uint8_t command) { outb(PS2_PORT_COMMAND, command); }

void ps2_send_data(uint8_t data) {
  if (ps2_can_send())
    outb(PS2_PORT_DATA, data);
}
bool ps2_can_send() {
  return !(inb(PS2_PORT_STATUS) & PS2_STATUS_INPUT_BUFFER_FULL);
}

void ps2_reset_cpu();

void ps2_init() {
  debug("Disabling port 1");
  ps2_send_command(PS2_COMMAND_DISABLE_PORT1);

  debug("Disabling port 2");
  ps2_send_command(PS2_COMMAND_DISABLE_PORT2);

  debug("Clearing controller data");
  while (ps2_data_available())
    ps2_read_data();

  debug("Getting CCB");
  ps2_send_command(PS2_COMMAND_READ_CCB);
  while (!ps2_data_available())
    asm("nop");
  uint8_t ccb = ps2_read_data();
  dual_channel = ccb & PS2_CCB_PORT2_CLOCK;
  ps2_send_command(PS2_COMMAND_WRITE_CCB);
  while (!ps2_can_send())
    asm("nop");

  debug("Disabling Interrupts + Scancode translation");
  ps2_send_data(ccb & ~(PS2_CCB_PORT1_INT | PS2_CCB_PORT2_INT |
                        PS2_CCB_PORT1_TRANSLATION));

  debug("Performing controller self-test");
  ps2_send_command(PS2_COMMAND_TEST_CONTROLLER);
  while (!ps2_data_available())
    asm("nop");
  uint8_t result = ps2_read_data();
  if (result != 0x55)
    kernel_panic_error("PS/2 Controller failed self-test");
  ps2_send_command(PS2_COMMAND_WRITE_CCB);
  while (!ps2_can_send())
    asm("nop");
  ps2_send_data(ccb & ~(PS2_CCB_PORT1_INT | PS2_CCB_PORT2_INT |
                        PS2_CCB_PORT1_TRANSLATION));
  if (dual_channel) {
    debug("Checking dual-channel");
    ps2_send_command(PS2_COMMAND_ENABLE_PORT2);
    io_wait();
    uint8_t ccb = ps2_read_data();
    dual_channel = !(ccb & PS2_CCB_PORT2_CLOCK);
    if (dual_channel)
      ps2_send_command(PS2_COMMAND_DISABLE_PORT2);
  }
  debug("Testing port 1");
  ps2_send_command(PS2_COMMAND_TEST_PORT1);
  while (!ps2_data_available())
    asm("nop");
  if (ps2_read_data()) {
    warn("WARNING: First PS/2 port failed, keyboard may "
         "not be available.");
  } else {
    port1_available = true;
  }
  if (dual_channel) {
    debug("Testing port 2");
    ps2_send_command(PS2_COMMAND_TEST_PORT2);
    while (!ps2_data_available())
      asm("nop");
    if (ps2_read_data()) {
      warn("WARNING: Second PS/2 port failed, mouse may "
           "not be available.");
    } else {
      port2_available = true;
    }
  }
  if (!(port1_available || port2_available)) {
    error("ERROR: All PS/2 ports failed, keyboard and mouse will not be "
          "available using PS/2.");
    return;
  }
  info("Finished initialization");
}
