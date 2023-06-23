#pragma once
#include <stdint.h>
#include "memory/pmm.h"

#define VMM_FLAG_PRESENT        (1 << 0)
#define VMM_FLAG_READ_WRITE     (1 << 1)
#define VMM_FLAG_SUPERUSER_PAGE (1 << 2)
#define VMM_FLAG_WRITETHROUGH   (1 << 3)
#define VMM_FLAG_CACHE_DISABLE  (1 << 4)
#define VMM_FLAG_ACCESSED       (1 << 5)
#define VMM_FLAG_GLOBAL         (1 << 8)
#define VMM_FLAG_XD             (1 << 63)

#define HH_ADDR(x) ((uintptr_t)(x) + get_hhdm())
#define LH_ADDR(x) ((uintptr_t)(x) & (~get_hhdm()))

struct vmm_invalidate_desc
{
    uint64_t pcid;
    uint64_t addr;
} __attribute__((packed));
typedef struct vmm_invalidate_desc vmm_invalidate_desc_t;

void init_virtual_memory();
void vmm_load_memmap(uintptr_t* pml4);
uintptr_t vmm_read_memmap();
void vmm_invalidate(vmm_invalidate_desc_t desc);
void vmm_map(pool_allocator_t* frame_allocator, uint64_t* pml4, uintptr_t phys, uintptr_t virt, uint16_t pcid, uint64_t flags);
void vmm_create_memmap(pool_allocator_t* frame_allocator, uint16_t pcid, uintptr_t** pml4);
uintptr_t get_hhdm();
uintptr_t get_kernel_offset();
uintptr_t vmm_alloc(pool_allocator_t* alloc, uintptr_t* pml4, uint16_t pcid, uintptr_t virtual_addr);