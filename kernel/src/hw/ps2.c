#include <hw/ps2.h>
#include <io/pio.h>

void ps2_send_command1(uint8_t);
void ps2_send_data1(uint8_t);

void ps2_send_command2(uint8_t);
void ps2_send_data2(uint8_t);

uint8_t ps2_read_data();
bool ps2_data_available() {
  return inb(PS2_PORT_STATUS) & PS2_STATUS_OUTPUT_BUFFER_FULL;
}

void ps2_send_data(uint8_t);
bool ps2_can_send() {
  return !(inb(PS2_PORT_STATUS) & PS2_STATUS_INPUT_BUFFER_FULL);
}

void ps2_reset_cpu();

void ps2_init();
