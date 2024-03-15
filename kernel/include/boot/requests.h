#pragma once

#include <boot/limine.h>
#include <stdint.h>

extern volatile uint64_t limine_base_revision[];

extern volatile struct limine_framebuffer_request framebuffer_request;
extern volatile struct limine_smp_request smp_request;
extern volatile struct limine_hhdm_request hhdm_request;
extern volatile struct limine_paging_mode_request paging_request;
extern volatile struct limine_memmap_request mmap_request;
