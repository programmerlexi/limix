#include <config.h>
#include <io/pio.h>
#include <io/serial/serial.h>
#include <stddef.h>

static bool _serial_available = false;

void serial_early_init() {
  outb(DEFAULT_COM + COM_INT_ENABLE, 0);
  outb(DEFAULT_COM + COM_LINE_CTRL, COM_LINE_CTRL_DLAB);
  outb(DEFAULT_COM + COM_DLAB_LSB_DIV, 0);
  outb(DEFAULT_COM + COM_DLAB_MSB_DIV, 3);
  outb(DEFAULT_COM + COM_LINE_CTRL, COM_LINE_CTRL_PAR_NONE |
                                        COM_LINE_CTRL_CHR_LEN_8 |
                                        COM_LINE_CTRL_STOP_BITS_1);
  outb(DEFAULT_COM + COM_FIFO_CTRL, COM_FIFO_ENABLE | COM_FIFO_LEN_14 |
                                        COM_FIFO_CLEAR_RECV |
                                        COM_FIFO_CLEAR_TRANSMIT);
  outb(DEFAULT_COM + COM_MODEM_CTRL, COM_MODEM_CTRL_RTS | COM_MODEM_CTRL_OUT1 |
                                         COM_MODEM_CTRL_OUT2 |
                                         COM_MODEM_CTRL_LOOP);
  outb(DEFAULT_COM + COM_DATA, 0xAE);
  if (inb(DEFAULT_COM + COM_DATA) != 0xAE)
    return;
  outb(DEFAULT_COM + COM_MODEM_CTRL, COM_MODEM_CTRL_DTR | COM_MODEM_CTRL_RTS |
                                         COM_MODEM_CTRL_OUT1 |
                                         COM_MODEM_CTRL_OUT2);
  _serial_available = true;
}
char serial_read() {
  if (!_serial_available)
    return 0;
  while (!serial_received())
    ;
  return inb(DEFAULT_COM + COM_DATA);
}
void serial_write(char c) {
  if (!_serial_available)
    return;
  while (!serial_can_send())
    ;
  return outb(DEFAULT_COM + COM_DATA, c);
}

bool serial_received() {
  return inb(DEFAULT_COM + COM_LINE_STATUS) & COM_LINE_STATUS_DR;
}
bool serial_can_send() {
  return inb(DEFAULT_COM + COM_LINE_STATUS) & COM_LINE_STATUS_THRE;
}

void serial_writes(char *s) {
  if (s == NULL)
    return;
  if (!_serial_available)
    return;
  while (*s) {
    serial_write(*s);
    s++;
  }
}
