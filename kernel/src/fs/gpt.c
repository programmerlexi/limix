#include "kernel/fs/gpt.h"
#include "kernel/hw/devman/devman.h"
#include "kernel/kernel.h"
#include "kernel/mm/mm.h"
#include "libk/printing.h"
#include "libk/utils/strings/strings.h"

void gpt_init(devman_storage_access_handle_t ah) {
  gpt_header_t *gpt = request_page_block(sizeof(gpt_header_t) / 0x1000);
  if (!devman_read(ah, 0, sizeof(gpt_header_t) / 512, gpt))
    kernel_panic_error("Couldn't read gpt");
  if (gpt->pmbr.partitions[0].attribute != 0)
    kernel_panic_error("Attributes aren't zero");
  if (gpt->pmbr.partitions[0].start_chs[0] != 0)
    kernel_panic_error("CHS[0] isn't zero");
  if (gpt->pmbr.partitions[0].start_chs[1] != 0x02)
    kernel_panic_error("CHS[1] isn't 0x02");
  if (gpt->pmbr.partitions[0].start_chs[2] != 0)
    kernel_panic_error("CHS[2] isn't zero");
  if (gpt->pmbr.partitions[0].partition_type != 0xee)
    kernel_panic_error("Type isn't 0xEE");

  if (!kstrncmp((char *)&gpt->partition_header.signature[0], "EFI PART", 8))
    kernel_panic_error("Signature invalid");

  kprint("GPT GUID: ");
  for (i32 i = 0; i < 8; i++) {
    kprintf("%w", ((u16 *)&gpt->partition_header.guid[0])[i]);
  }
  kprint("\n\r");

  kprintf("GPT partition entries: %u\n\r",
          gpt->partition_header.partition_entries);
}
