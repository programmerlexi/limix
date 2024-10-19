#include <kernel/boot/requests.h>
#include <kernel/constructors.h>
#include <kernel/debug.h>
#include <kernel/hw/acpi/acpi.h>
#include <kernel/initgraph.h>
#include <kernel/kernel.h>
#include <kernel/mm/hhtp.h>
#include <lai/core.h>
#include <libk/utils/strings/strings.h>
#include <stdbool.h>
#include <stddef.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "acpi"

CONSTRUCTOR(acpi) { INITGRAPH_NODE("acpi", acpi_init); }

static bool initialized = false;
static bool got_xsdt = false;
static AcpiXsdt *xsdt;
static AcpiRsdt *rsdt;
static u64 revision;

void acpi_init() {
  AcpiRsdp *rsdp = g_rsdp_request.response->address;
  if (!kstrncmp(rsdp->signature, "RSD PTR ", 8))
    kernel_panic_error("Broken acpi root pointer");
  revision = (u64)rsdp->revision;
  logf(LOGLEVEL_INFO, "ACPI revision: %u", revision);
  if (rsdp->revision != 2) {
    log(LOGLEVEL_WARN0, "Using ACPI 1.0");
    if (!rsdp->rsdt_address)
      kernel_panic_error("RSDT doesn't exist");
    rsdt = (AcpiRsdt *)HHDM(rsdp->rsdt_address);
  } else {
    got_xsdt = true;
    xsdt = (AcpiXsdt *)HHDM(rsdp->xsdt_address);
  }
  initialized = true;
  lai_set_acpi_revision(revision);
  lai_create_namespace();
}

bool acpi_is_v2() { return revision; }

static AcpiTable *acpi_get32(char *id) {
  u32 entries = (rsdt->header.length - sizeof(rsdt->header)) / 4;
  for (u32 i = 0; i < entries; i++) {
    AcpiTable *table = (AcpiTable *)HHDM(rsdt->other_sdt[i]);
    if (kstrncmp(table->signature, id, 4))
      return (AcpiTable *)HHDM(table);
  }
  return NULL;
}
static AcpiTable *acpi_get64(char *id) {
  u64 entries = (xsdt->header.length - sizeof(xsdt->header)) / 8;
  for (u64 i = 0; i < entries; i++) {
    AcpiTable *table = (AcpiTable *)HHDM(xsdt->other_sdt[i]);
    if (kstrncmp(table->signature, id, 4))
      return (AcpiTable *)HHDM(table);
  }
  return NULL;
}
AcpiTable *acpi_get(char *id) {
  if (!initialized)
    return NULL;
  if (got_xsdt)
    return acpi_get64(id);
  return acpi_get32(id);
}
