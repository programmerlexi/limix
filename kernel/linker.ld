/* Tell the linker that we want an x86_64 ELF64 output file */
OUTPUT_FORMAT(elf64-x86-64)
 
/* We want the symbol _start to be our entry point */
ENTRY(_start)
 
/* Define the program headers we want so the bootloader gives us the right */
/* MMU permissions */
PHDRS {
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ; /* Execute + Read */
    rodata  PT_LOAD    FLAGS((1 << 2)) ;            /* Read only */
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ; /* Write + Read */
    dynamic PT_DYNAMIC FLAGS((1 << 1) | (1 << 2)) ; /* Dynamic PHDR for relocations */
    requests PT_LOAD;
}
 
SECTIONS {
    /* We wanna be placed in the topmost 2GiB of the address space, for optimisations */
    /* and because that is what the Limine spec mandates. */
    /* Any address in this region will do, but often 0xffffffff80000000 is chosen as */
    /* that is the beginning of the region. */
    . = 0xffffffff80000000;

    .requests : {
        KEEP(*(.requests_start_marker))
        KEEP(*(.requests))
        KEEP(*(.requests_end_marker))
    } :requests

    . = ALIGN(CONSTANT(MAXPAGESIZE));
 
    .text : {
        *(.text .text.*)
    } :text
 
    . = ALIGN(CONSTANT(MAXPAGESIZE));
 
    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    .init_array : {
        __init_array = .;
        *(.init_array .init_array.*)
        *(.ctors .ctors.*)
        __init_array_end = .;
    } :rodata
 
    . = ALIGN(CONSTANT(MAXPAGESIZE));
 
    .data : {
        *(.data .data.*)
        KEEP(*(.requests_start_marker))
        KEEP(*(.requests))
        KEEP(*(.requests_end_marker))
    } :data
 
    .dynamic : {
        *(.dynamic)
    } :data :dynamic
 
    _bss_start = .;
    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data
    _bss_end = .;
 
    /DISCARD/ : {
        *(.eh_frame)
        *(.note .note.*)
    }
}
