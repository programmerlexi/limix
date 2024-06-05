#include "kernel/hw/acpi/acpi.h"
#include "kernel/boot/requests.h"
#include "kernel/debug.h"
#include "kernel/kernel.h"
#include "kernel/mm/hhtp.h"
#include "libk/utils/strings/strings.h"
#include <stdbool.h>
#include <stddef.h>

static bool initialized = false;
static bool got_xsdt = false;
static acpi_xsdt_t *xsdt;
static acpi_rsdt_t *rsdt;

void acpi_init() {
  acpi_rsdp_t *rsdp = g_rsdp_request.response->address;
  if (!kstrncmp(rsdp->signature, "RSD PTR ", 8))
    kernel_panic_error("Broken acpi root pointer");
  u64 revision = (u64)rsdp->revision;
  logf(LOGLEVEL_INFO, "ACPI revision: %u", revision);
  if (rsdp->revision != 2) {
    log(LOGLEVEL_WARN0, "Using ACPI 1.0");
    if (!rsdp->rsdt_address)
      kernel_panic_error("RSDT doesn't exist");
    rsdt = (acpi_rsdt_t *)HHDM(rsdp->rsdt_address);
  } else {
    got_xsdt = true;
    xsdt = (acpi_xsdt_t *)HHDM(rsdp->xsdt_address);
  }
  initialized = true;
}

static acpi_table_t *acpi_get32(char *id) {
  u32 entries = (rsdt->header.length - sizeof(rsdt->header)) / 4;
  for (u32 i = 0; i < entries; i++) {
    acpi_table_t *table = (acpi_table_t *)HHDM(rsdt->other_sdt[i]);
    if (kstrncmp(table->signature, id, 4))
      return (acpi_table_t *)HHDM(table);
  }
  return NULL;
}
static acpi_table_t *acpi_get64(char *id) {
  u64 entries = (xsdt->header.length - sizeof(xsdt->header)) / 8;
  for (u64 i = 0; i < entries; i++) {
    acpi_table_t *table = (acpi_table_t *)HHDM(xsdt->other_sdt[i]);
    if (kstrncmp(table->signature, id, 4))
      return (acpi_table_t *)HHDM(table);
  }
  return NULL;
}
acpi_table_t *acpi_get(char *id) {
  if (!initialized)
    return NULL;
  if (got_xsdt)
    return acpi_get64(id);
  return acpi_get32(id);
}
