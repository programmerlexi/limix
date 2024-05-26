#pragma once

#include "limine.h"

extern volatile uint64_t limine_base_revision[];

extern volatile struct limine_framebuffer_request g_framebuffer_request;
extern volatile struct limine_smp_request g_smp_request;
extern volatile struct limine_hhdm_request g_hhdm_request;
extern volatile struct limine_paging_mode_request g_paging_request;
extern volatile struct limine_memmap_request g_mmap_request;
extern volatile struct limine_module_request g_module_request;
extern volatile struct limine_kernel_address_request g_kernel_address_request;
