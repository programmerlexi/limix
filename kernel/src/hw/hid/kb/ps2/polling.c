#define DEBUG_MODULE "kb_poll"

#include <debug.h>
#include <hw/hid/kb/kb.h>
#include <hw/hid/kb/poll.h>
#include <hw/ps2.h>
#include <io/pio.h>
#include <printing.h>

static uint8_t kb_set = 0;

void kb_init_polling() {
  bool success = false;
  while (!success) {
    debug("Resetting keyboard");
    ps2_send_command1(0xff);
    io_wait();
    if (ps2_read_data() != 0xFA)
      continue;
    io_wait();
    if (ps2_read_data() != 0xAA)
      continue;
    success = true;
  }
  debug("Reset keyboard");
  debug("Turning off LEDs");
set_leds:
  ps2_send_command1(0xED);
  io_wait();
  ps2_send_data1(0);
  io_wait();
  if (ps2_read_data() == 0xFE)
    goto set_leds;
  debug("Reading scancode set");
  ps2_send_command1(0xF0);
  io_wait();
  ps2_send_data1(0);
  io_wait();
  if (ps2_read_data() == 0xFE) {
    io_wait();
    uint8_t scancode_set = ps2_read_data();
    switch (scancode_set) {
    case 0x43:
      kb_set = 1;
      break;
    case 0x41:
      kb_set = 2;
      break;
    case 0x3f:
      kb_set = 3;
      break;
    default:
      kprintf(debug_str "Unrecognized set: %x\n\r", (uint32_t)scancode_set);
    }
  } else
    kb_set = ps2_read_data();
  if (kb_set == 2) {
    warn("Scancode set 2 has missing features");
  }
  if (kb_set == 1) {
    warn("Scancode set 1 is not fully implemented");
  }
  if (kb_set == 3) {
    error("Scancode set 3 is not supported");
  }
  info("Keyboard done.");
}
