#pragma once
#include <stdint.h>
#include <stddef.h>

#include "memory_mgmt.h"

#define VMM_FLAG_PRESENT        (1 << 0)
#define VMM_FLAG_READ_WRITE     (1 << 1)
#define VMM_FLAG_USER_PAGE      (1 << 2)
#define VMM_FLAG_WRITETHROUGH   (1 << 3)
#define VMM_FLAG_CACHE_DISABLE  (1 << 4)
#define VMM_FLAG_ACCESSED       (1 << 5)
#define VMM_FLAG_LARGE          (1 << 7)
#define VMM_FLAG_GLOBAL         (1 << 8)
#define VMM_FLAG_XD             (1 << 63)

#define VMM_SIZE_KB             0x1000
#define VMM_SIZE_MB             0x100000
#define VMM_SIZE_GB             0x40000000

struct vmm_invalidate_desc
{
    uint64_t pcid;
    uint64_t addr;
} __attribute__((packed));

struct vmm_mapping_desc
{
    uintptr_t paddr;
    uintptr_t vaddr;
    uint32_t size;
    uint64_t flags;
    uint16_t pcid;
    uintptr_t pml4;
};

void vmm_map(struct vmm_mapping_desc desc, struct pool_allocator* page_allocator);
void vmm_alloc(struct vmm_mapping_desc desc, struct pool_allocator* page_allocator);
void vmm_invalidate(struct vmm_invalidate_desc desc);
void vmm_load_map(uintptr_t pml4);
void vmm_make_map(uintptr_t* pml4, uint16_t pcid, struct pool_allocator* page_allocator);
uintptr_t vmm_read_map();