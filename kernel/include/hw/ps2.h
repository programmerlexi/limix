#pragma once

#include <stdbool.h>
#include <stdint.h>

#define PS2_PORT_DATA 0x60
#define PS2_PORT_STATUS 0x64
#define PS2_PORT_COMMAND 0x64

#define PS2_STATUS_OUTPUT_BUFFER_FULL 1 << 0
#define PS2_STATUS_INPUT_BUFFER_FULL 1 << 1
#define PS2_STATUS_SYSTEM_FLAG 1 << 2
#define PS2_STATUS_COMMAND 1 << 3
#define PS2_STATUS_TIME_OUT 1 << 6
#define PS2_STATUS_PARITY 1 << 7

#define PS2_COMMAND_READ_CCB 0x20
#define PS2_COMMAND_WRITE_CCB 0x60
#define PS2_COMMAND_DISABLE_PORT2 0xA7
#define PS2_COMMAND_ENABLE_PORT2 0xA8
#define PS2_COMMAND_TEST_PORT2 0xA9
#define PS2_COMMAND_TEST_CONTROLLER 0xAA
#define PS2_COMMAND_TEST_PORT1 0xAB
#define PS2_COMMAND_DISABLE_PORT1 0xAD
#define PS2_COMMAND_ENABLE_PORT1 0xAE
#define PS2_COMMAND_READ_CONTROLLER_OUTPUT_PORT 0xD0
#define PS2_COMMAND_WRITE_CONTROLLER_OUTPUT_PORT 0xD1
#define PS2_COMMAND_WRITE_PORT1_OUTPUT 0xD2
#define PS2_COMMAND_WRITE_PORT2_OUTPUT 0xD3
#define PS2_COMMAND_WRITE_PORT2_INPUT 0xD4
#define PS2_COMMAND_RESET_CPU 0xFE

#define PS2_CCB_PORT1_INT 1 << 0
#define PS2_CCB_PORT2_INT 1 << 1
#define PS2_CCB_SYSTEM_FLAG 1 << 2
#define PS2_CCB_PORT1_CLOCK 1 << 4
#define PS2_CCB_PORT2_CLOCK 1 << 5
#define PS2_CCB_PORT1_TRANSLATION 1 << 6

#define PS2_COP_RESET 1 << 0
#define PS2_COP_A20 1 << 1
#define PS2_COP_PORT2_CLOCK 1 << 2
#define PS2_COP_PORT2_DATA 1 << 3
#define PS2_COP_FULL_PORT1 1 << 4
#define PS2_COP_FULL_PORT2 1 << 5
#define PS2_COP_PORT1_CLOCK 1 << 6
#define PS2_COP_PORT1_DATA 1 << 7

void ps2_send_command1(uint8_t cmd);
void ps2_send_data1(uint8_t data);

void ps2_send_command2(uint8_t cmd);
void ps2_send_data2(uint8_t data);
uint8_t ps2_read_data2();

uint8_t ps2_read_data();
bool ps2_data_available();

void ps2_send_command(uint8_t cmd);

void ps2_send_data(uint8_t data);
bool ps2_can_send();

void ps2_reset_cpu();

void ps2_init();
