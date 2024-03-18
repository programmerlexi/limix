#include <gfx/vt/vt.h>
#include <hw/ps2.h>
#include <io/pio.h>
#include <kernel.h>

bool dual_channel = true;
bool port1_available = false;
bool port2_available = false;

void ps2_send_command1(uint8_t);
void ps2_send_data1(uint8_t);

void ps2_send_command2(uint8_t);
void ps2_send_data2(uint8_t);

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
  ps2_send_command(PS2_COMMAND_DISABLE_PORT1);
  ps2_send_command(PS2_COMMAND_DISABLE_PORT2);
  while (!ps2_data_available())
    ps2_read_data();
  ps2_send_command(PS2_COMMAND_READ_CCB);
  while (!ps2_data_available())
    asm("nop");
  uint8_t ccb = ps2_read_data();
  dual_channel = ccb & PS2_CCB_PORT2_CLOCK;
  ps2_send_command(PS2_COMMAND_WRITE_CCB);
  while (!ps2_can_send())
    asm("nop");
  ps2_send_data(ccb & ~(PS2_CCB_PORT1_INT | PS2_CCB_PORT2_INT |
                        PS2_CCB_PORT1_TRANSLATION));
  ps2_send_command(PS2_COMMAND_TEST_CONTROLLER);
  while (!ps2_data_available())
    asm("nop");
  uint8_t result = ps2_read_data();
  if (result != 0xAA)
    kernel_panic_error("PS/2 Controller failed self-test");
  ps2_send_command(PS2_COMMAND_WRITE_CCB);
  while (!ps2_can_send())
    asm("nop");
  ps2_send_data(ccb & ~(PS2_CCB_PORT1_INT | PS2_CCB_PORT2_INT |
                        PS2_CCB_PORT1_TRANSLATION));
  if (dual_channel) {
    ps2_send_command(PS2_COMMAND_ENABLE_PORT2);
    while (!ps2_data_available())
      asm("nop");
    uint8_t ccb = ps2_read_data();
    dual_channel = !(ccb & PS2_CCB_PORT2_CLOCK);
    if (dual_channel)
      ps2_send_command(PS2_COMMAND_DISABLE_PORT2);
  }
  ps2_send_command(PS2_COMMAND_TEST_PORT1);
  while (!ps2_data_available())
    asm("nop");
  if (ps2_read_data()) {
    kprint(
        "WARNING: First PS/2 port failed, keyboard may not be available.\n\r");
  } else {
    port1_available = true;
  }
  if (dual_channel) {
    ps2_send_command(PS2_COMMAND_TEST_PORT2);
    while (!ps2_data_available())
      asm("nop");
    if (ps2_read_data()) {
      kprint(
          "WARNING: Second PS/2 port failed, mouse may not be available.\n\r");
    } else {
      port2_available = true;
    }
  }
  if (!(port1_available || port2_available)) {
    kprint("ERROR: All PS/2 ports failed, keyboard and mouse will not be "
           "available using PS/2.\n\r");
    return;
  }
}
