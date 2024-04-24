#pragma once
#include <stdbool.h>

/* Reliable ports */
#define COM1 0x3f8
#define COM2 0x2f8

/* Unreliable ports */
#define COM3 0x3e8
#define COM4 0x2e8
#define COM5 0x5f8
#define COM6 0x4f8
#define COM7 0x5e8
#define COM8 0x4e8

/* Offsets */
#define COM_DATA 0
#define COM_DLAB_LSB_DIV 0

#define COM_INT_ENABLE 1
#define COM_DLAB_MSB_DIV 1

#define COM_FIFO_CTRL 2
#define COM_LINE_CTRL 3
#define COM_MODEM_CTRL 4
#define COM_LINE_STATUS 5
#define COM_MODEM_STATUS 6
#define COM_SCRATCH 7

/* Bit fields */
#define COM_FIFO_ENABLE 1
#define COM_FIFO_CLEAR_RECV 2
#define COM_FIFO_CLEAR_TRANSMIT 4
#define COM_FIFO_DMA_MODE 8
#define COM_FIFO_LEN_1 0
#define COM_FIFO_LEN_4 0x40
#define COM_FIFO_LEN_8 0x80
#define COM_FIFO_LEN_14 0xB0

#define COM_LINE_CTRL_DLAB 1 << 7

#define COM_LINE_CTRL_CHR_LEN_5 0
#define COM_LINE_CTRL_CHR_LEN_6 1
#define COM_LINE_CTRL_CHR_LEN_7 2
#define COM_LINE_CTRL_CHR_LEN_8 3

#define COM_LINE_CTRL_STOP_BITS_1 0
#define COM_LINE_CTRL_STOP_BITS_2 4

#define COM_LINE_CTRL_PAR_NONE 0
#define COM_LINE_CTRL_PAR_ODD 8
#define COM_LINE_CTRL_PAR_EVEN 24
#define COM_LINE_CTRL_PAR_MARK 40
#define COM_LINE_CTRL_PAR_SPACE 56

#define COM_INT_DATA 1
#define COM_INT_TRANSMITTER_EMPTY 2
#define COM_INT_BREAK_OR_ERROR 4
#define COM_INT_STATUS 8

#define COM_MODEM_CTRL_DTR 1
#define COM_MODEM_CTRL_RTS 2
#define COM_MODEM_CTRL_OUT1 4
#define COM_MODEM_CTRL_OUT2 8
#define COM_MODEM_CTRL_LOOP 16

#define COM_LINE_STATUS_DR 1
#define COM_LINE_STATUS_OE 2
#define COM_LINE_STATUS_PE 4
#define COM_LINE_STATUS_FE 8
#define COM_LINE_STATUS_BI 16
#define COM_LINE_STATUS_THRE 32
#define COM_LINE_STATUS_TEMT 64
#define COM_LINE_STATUS_IMPENDING_ERROR 128

#define COM_MODEM_STATUS_DCTS 1
#define COM_MODEM_STATUS_DDSR 2
#define COM_MODEM_STATUS_TERI 4
#define COM_MODEM_STATUS_DDCD 8
#define COM_MODEM_STATUS_CTS 16
#define COM_MODEM_STATUS_DSR 32
#define COM_MODEM_STATUS_RI 64
#define COM_MODEM_STATUS_DCD 128

void serial_early_init();
bool serial_received();
char serial_read();
bool serial_can_send();
void serial_write(char c);
void serial_writes(char *str);
