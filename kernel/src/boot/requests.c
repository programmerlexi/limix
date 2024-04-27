#include <boot/limine.h>
#include <config.h>
#include <stddef.h>

volatile LIMINE_BASE_REVISION(1);

volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0, .response = NULL};

volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST, .revision = 0, .response = NULL};

volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST, .revision = 0, .response = NULL};

volatile struct limine_paging_mode_request paging_request = {
    .id = LIMINE_PAGING_MODE_REQUEST,
    .revision = 0,
    .mode = USED_PAGING_MODE,
    .response = NULL};

volatile struct limine_memmap_request mmap_request = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = NULL};

volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST, .revision = 0, .response = NULL};
