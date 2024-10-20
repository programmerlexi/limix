#include <kernel/boot/requests.h>
#include <kernel/config.h>
#include <limine.h>
#include <stddef.h>

__attribute__((used, section(".requests"))) volatile LIMINE_BASE_REVISION(2);

__attribute__((used,
               section(".requests"))) volatile struct limine_framebuffer_request
    g_framebuffer_request = {
        .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0, .response = NULL};

__attribute__((used, section(".requests"))) volatile struct limine_smp_request
    g_smp_request = {.id = LIMINE_SMP_REQUEST, .revision = 0, .response = NULL};

__attribute__((used, section(".requests"))) volatile struct limine_hhdm_request
    g_hhdm_request = {
        .id = LIMINE_HHDM_REQUEST, .revision = 0, .response = NULL};

__attribute__((used,
               section(".requests"))) volatile struct limine_paging_mode_request
    g_paging_request = {.id = LIMINE_PAGING_MODE_REQUEST,
                        .revision = 0,
                        .mode = USED_PAGING_MODE,
                        .max_mode = USED_PAGING_MODE,
                        .min_mode = USED_PAGING_MODE,
                        .response = NULL};

__attribute__((
    used,
    section(
        ".requests"))) volatile struct limine_memmap_request g_mmap_request = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = NULL};

__attribute__((used,
               section(".requests"))) volatile struct limine_module_request
    g_module_request = {
        .id = LIMINE_MODULE_REQUEST, .revision = 0, .response = NULL};

__attribute__((
    used, section(".requests"))) volatile struct limine_kernel_address_request
    g_kernel_address_request = {
        .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0, .response = NULL};

__attribute__((used, section(".requests"))) volatile struct limine_rsdp_request
    g_rsdp_request = {
        .id = LIMINE_RSDP_REQUEST, .revision = 0, .response = NULL};

__attribute__((used,
               section(".requests"))) volatile struct limine_smbios_request
    g_smbios_request = {
        .id = LIMINE_SMBIOS_REQUEST, .revision = 0, .response = NULL};

__attribute__((used,
               section(".requests_start_"
                       "marker"))) static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((
    used,
    section(
        ".requests_end_marker"))) static volatile LIMINE_REQUESTS_END_MARKER;
