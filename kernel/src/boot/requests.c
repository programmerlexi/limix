#include "kernel/config.h"
#include "limine.h"
#include <stddef.h>

volatile LIMINE_BASE_REVISION(1);

volatile struct limine_framebuffer_request g_framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0, .response = NULL};

volatile struct limine_smp_request g_smp_request = {
    .id = LIMINE_SMP_REQUEST, .revision = 0, .response = NULL};

volatile struct limine_hhdm_request g_hhdm_request = {
    .id = LIMINE_HHDM_REQUEST, .revision = 0, .response = NULL};

volatile struct limine_paging_mode_request g_paging_request = {
    .id = LIMINE_PAGING_MODE_REQUEST,
    .revision = 0,
    .mode = USED_PAGING_MODE,
    .response = NULL};

volatile struct limine_memmap_request g_mmap_request = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = NULL};

volatile struct limine_module_request g_module_request = {
    .id = LIMINE_MODULE_REQUEST, .revision = 0, .response = NULL};

volatile struct limine_kernel_address_request g_kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0, .response = NULL};
