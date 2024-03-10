#include <config.h>
#include <pio.h>
#include <serial.h>
#include <stddef.h>

bool serial_available = false;

void serial_early_init() {
  /* Turn off interrupts */
  outb(DEFAULT_COM + COM_INT_ENABLE, 0);
  /* Setup for 38400 baud */
  outb(DEFAULT_COM + COM_LINE_CTRL, COM_LINE_CTRL_DLAB);
  outb(DEFAULT_COM + COM_DLAB_LSB_DIV, 0);
  outb(DEFAULT_COM + COM_DLAB_MSB_DIV, 3);
  /* Disable DLAB, set char length = 8, no parity and 1 stop bit */
  outb(DEFAULT_COM + COM_LINE_CTRL, COM_LINE_CTRL_PAR_NONE |
                                        COM_LINE_CTRL_CHR_LEN_8 |
                                        COM_LINE_CTRL_STOP_BITS_1);
  /* Enable FIFO */
  outb(DEFAULT_COM + COM_FIFO_CTRL, COM_FIFO_ENABLE | COM_FIFO_LEN_14 |
                                        COM_FIFO_CLEAR_RECV |
                                        COM_FIFO_CLEAR_TRANSMIT);
  /* Make sure the UART is OK */
  outb(DEFAULT_COM + COM_MODEM_CTRL, COM_MODEM_CTRL_RTS | COM_MODEM_CTRL_OUT1 |
                                         COM_MODEM_CTRL_OUT2 |
                                         COM_MODEM_CTRL_LOOP);
  outb(DEFAULT_COM + COM_DATA, 0xAE);
  if (inb(DEFAULT_COM + COM_DATA) != 0xAE)
    return;
  /* Set UART into usable state */
  outb(DEFAULT_COM + COM_MODEM_CTRL, COM_MODEM_CTRL_DTR | COM_MODEM_CTRL_RTS |
                                         COM_MODEM_CTRL_OUT1 |
                                         COM_MODEM_CTRL_OUT2);
  serial_available = true;
}
char serial_read() {
  if (!serial_available)
    return 0;
  while (!serial_received())
    ;
  return inb(DEFAULT_COM + COM_DATA);
}
void serial_write(char c) {
  if (!serial_available)
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
  if (!serial_available)
    return;
  while (*s) {
    serial_write(*s);
    s++;
  }
}
