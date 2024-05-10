#define DEBUG_MODULE "kb_poll"

#include <debug.h>
#include <hw/hid/kb/kb.h>
#include <hw/hid/kb/poll.h>
#include <hw/ps2.h>
#include <io/pio.h>
#include <printing.h>

static u8 _kb_set = 0;

void kb_init_polling() {
  bool success = false;
  while (!success) {
    log(LOGLEVEL_ANALYZE, "Resetting keyboard");
    ps2_send_command1(0xff);
    io_wait();
    if (ps2_read_data() != 0xFA)
      continue;
    io_wait();
    if (ps2_read_data() != 0xAA)
      continue;
    success = true;
  }
  log(LOGLEVEL_ANALYZE, "Reset keyboard");
  log(LOGLEVEL_ANALYZE, "Turning off LEDs");
set_leds:
  ps2_send_command1(0xED);
  io_wait();
  ps2_send_data1(0);
  io_wait();
  if (ps2_read_data() == 0xFE)
    goto set_leds;
  log(LOGLEVEL_ANALYZE, "Reading scancode set");
  ps2_send_command1(0xF0);
  io_wait();
  ps2_send_data1(0);
  io_wait();
  if (ps2_read_data() == 0xFE) {
    io_wait();
    u8 scancode_set = ps2_read_data();
    switch (scancode_set) {
    case 0x43:
      _kb_set = 1;
      break;
    case 0x41:
      _kb_set = 2;
      break;
    case 0x3f:
      _kb_set = 3;
      break;
    default:
      logf(LOGLEVEL_ERROR, "Unrecognized set: %x", (u32)scancode_set);
    }
  } else
    _kb_set = ps2_read_data();
  if (_kb_set == 2) {
    log(LOGLEVEL_WARN1, "Scancode set 2 has missing features");
  }
  if (_kb_set == 1) {
    log(LOGLEVEL_WARN1, "Scancode set 1 is not fully implemented");
  }
  if (_kb_set == 3) {
    log(LOGLEVEL_CRITICAL, "Scancode set 3 is not supported");
  }
  log(LOGLEVEL_INFO, "Keyboard done.");
}
