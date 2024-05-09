#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <types.h>

typedef u64 pml4e_t;
typedef u64 pdpe_t;
typedef u64 pde_t;
typedef u64 pte_t;

typedef u64 *pml4_t;
typedef u64 *pdp_t;
typedef u64 *pd_t;
typedef u64 *pt_t;

typedef void *vaddr_t;
typedef void *paddr_t;

#define PTE_PRESENT 1 << 0
#define PTE_WRITABLE 1 << 1
#define PTE_SUPERVISOR 1 << 2
#define PTE_WRITE_THROUGH 1 << 3
#define PTE_CACHE_DISABLE 1 << 4
#define PTE_ACCESSED 1 << 5
#define PTE_AVL0 1 << 6
#define PTE_PAGE_SIZE 1 << 7
#define PTE_AVL1 1 << 8
#define PTE_AVL2 1 << 9
#define PTE_AVL3 1 << 10
#define PTE_ADDR(x) (0xfffffffff000 & x)
#define PTE_NX 1 << 63

typedef struct virtual_address_space {
  bool allow_kas_changes;
  bool exists;
  pml4_t pml4;
} virtual_address_space_t;

void init_kernel_vas();
virtual_address_space_t init_new_vas();
virtual_address_space_t clone_vas(virtual_address_space_t vas);

bool kmmap(vaddr_t virt, paddr_t phys);
bool is_kmapped(vaddr_t virt);
bool kunmap(vaddr_t virt);
paddr_t kmapping(vaddr_t virt);

bool mmap(virtual_address_space_t vas, vaddr_t virt, paddr_t phys);
bool is_mapped(virtual_address_space_t vas, vaddr_t virt);
bool munmap(virtual_address_space_t vas, vaddr_t virt);
paddr_t mapping(virtual_address_space_t vas, vaddr_t virt);
