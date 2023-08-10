#include "memory_mgmt.h"
#include "utils.h"
#include "capabilities.h"

uint8_t pcid_enabled;
uintptr_t* vmm_kernel_map;

static inline size_t vmm_setup_level(uintptr_t* level, size_t index, uintptr_t flags, struct pool_allocator* page_allocator)
{
    if(!(level[index]) & VMM_FLAG_PRESENT)
    {
        uintptr_t frame = pool_fetch_zero(page_allocator);
        if(!frame)
            return 0;
        
        level[index] = frame | flags | VMM_FLAG_PRESENT;
    }

    return 1;
}

void vmm_map(struct vmm_mapping_desc desc, struct pool_allocator* page_allocator)
{
    if(!desc.pml4)
        return;

    if((desc.paddr % desc.size != 0) || (desc.vaddr % desc.size != 0))
        return;

    if(page_allocator->item_size != PMM_PAGE_SIZE)
        return;

    size_t levels;
    switch(desc.size)
    {
        case VMM_SIZE_KB:
            levels = 3;
            break;
        case VMM_SIZE_MB:
            levels = 2;
            break;
        case VMM_SIZE_GB:
            if(!CPU_CAPABILITY_GIGAPAGE_PRESENT)
                return;

            levels = 1;
            break;
        default:
            return; 
    }

    uintptr_t* level_ptr = (uintptr_t*)(HH_ADDR(desc.pml4) & 0xFFFFFFFFFFFFF000);
    size_t index = (desc.vaddr >> 39) & 0x1FF; //pml4 index

    const offsets[3] = { 30, 21, 12 };

    for(size_t i = 0; i < levels; i++)
    {
        if(!vmm_setup_level(level_ptr, index, desc.flags, page_allocator))
            return;

        level_ptr = (uintptr_t*)HH_ADDR(level_ptr[index] & 0x0000FFFFF000);
        index = (desc.vaddr >> offsets[i]) & 0x1FF;
    }

    index &= 0x1FF;

    // Check is page already mapped
    if(level_ptr[index] & VMM_FLAG_PRESENT)
        __builtin_trap();

    level_ptr[index] = desc.paddr | desc.flags | VMM_FLAG_PRESENT;
    if(desc.size != VMM_SIZE_KB)
        level_ptr[index] |= VMM_FLAG_LARGE;
}

void vmm_alloc(struct vmm_mapping_desc desc, struct pool_allocator* page_allocator)
{
    desc.vaddr = ALIGN_DOWN(desc.vaddr, PMM_PAGE_SIZE);
    desc.paddr = (uintptr_t)pool_fetch(page_allocator);
    if(desc.paddr)
    {
        __builtin_trap();
        return;
    }

    vmm_map(desc, page_allocator);
}

void vmm_invalidate(struct vmm_invalidate_desc desc)
{

}

void vmm_load_map(uintptr_t pml4)
{
    __asm__ volatile ("mov %0, %%cr3" :: "r"(pml4));
}

void vmm_make_map(uintptr_t* pml4, uint16_t pcid, struct pool_allocator* page_allocator)
{
    uintptr_t* pml4_alloc = pool_fetch_zero(page_allocator);
    if(pcid_enabled)
    {
        *pml4 = (uintptr_t)pml4_alloc | (pcid & 0xFFF);
    }
    else
    {
        *pml4 = (uintptr_t)pml4_alloc;
    }

    pml4_alloc = (uintptr_t*)HH_ADDR(pml4_alloc);
    uintptr_t* kernel_map = (uintptr_t*)HH_ADDR(vmm_kernel_map);
    for(size_t i = 256; i < 512; i++)
    {
        pml4_alloc[i] = kernel_map[i];
    }
}

uintptr_t vmm_read_map()
{
    uint64_t addr;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(addr));
    return addr;
}
