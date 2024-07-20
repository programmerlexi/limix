#include "kernel/hw/smbios/smbios.h"

void smb_init() {
  // TODO: Write a function to init SMB stuff
}
usz smb_struct_len(smb_header_t *h) {
  usz i;
  const char *strtab = (char *)h + h->length;
  for (i = 1; strtab[i - 1] != '\0' || strtab[i] != '\0'; i++)
    ;
  return h->length + i + 1;
}
