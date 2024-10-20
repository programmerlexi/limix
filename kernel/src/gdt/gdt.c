#include <kernel/gdt/gdt.h>
#include <libk/types.h>

__attribute__((aligned(0x1000))) Gdt g_gdt = {
    {0, 0, 0, 0x00, 0x00, 0}, // null
    {0xff, 0, 0,
     GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_PRESENT | GDT_ACCESS_CODE_READABLE |
         GDT_ACCESS_RING0,
     GDT_FLAG_64BIT | GDT_FLAG_GRANULARITY_4K | 0xf, 0}, // kernel code segment
    {0xff, 0, 0,
     GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE |
         GDT_ACCESS_DATA_DIRECTION_NORMAL | GDT_ACCESS_PRESENT |
         GDT_ACCESS_RING0,
     GDT_FLAG_64BIT | GDT_FLAG_GRANULARITY_4K | 0xf, 0}, // kernel data segment
    {0, 0, 0, 0x00, 0x00, 0},                            // user null
    {0xff, 0, 0,
     GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_PRESENT | GDT_ACCESS_CODE_READABLE |
         GDT_ACCESS_RING3,
     GDT_FLAG_64BIT | GDT_FLAG_GRANULARITY_1B | 0xf, 0}, // user code segment
    {0xff, 0, 0,
     GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE |
         GDT_ACCESS_DATA_DIRECTION_NORMAL | GDT_ACCESS_PRESENT |
         GDT_ACCESS_RING3,
     GDT_FLAG_64BIT | GDT_FLAG_GRANULARITY_1B | 0xf, 0}, // user data segment
};

void gdt_init(void) {
  __attribute__((aligned(0x1000))) GdtDescriptor gdt_desc;
  gdt_desc.size = sizeof(Gdt) - 1;
  gdt_desc.offset = (uptr)&g_gdt;
  load_gdt(&gdt_desc);
}
