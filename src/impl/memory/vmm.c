#include "limine.h"
#include "memory/vmm.h"
#include "utils.h"
#include "capabilities.h"

#define ENTRY_ADDR(x)       ((uintptr_t*)(((x) & 0x0000FFFFF000) + get_hhdm()))
#define VMM_PAGE_ADDRESS(x) ((uintptr_t)x & 0xFFFFFFFFF000)

extern struct limine_hhdm_request hhdm_request;
extern struct limine_memmap_request mmap_request;
extern struct limine_kernel_address_request kernel_addr_request;

extern uint8_t kaddr_end[], kaddr_start[];

/*static*/ uintptr_t* vmm_pml_kernel;

static uint8_t pcid_supported = 0;

struct page_index
{
    uint16_t pml4e;
    uint16_t pdpte;
    uint16_t pde;
    uint16_t pte;
};

struct page_index get_page_indices(uintptr_t virtual)
{
    struct page_index result;
    virtual >>= 12;

    result.pte = (virtual & 0x1FF);
    virtual >>= 9;

    result.pde = (virtual & 0x1FF);
    virtual >>= 9;

    result.pdpte = (virtual & 0x1FF);
    virtual >>= 9;

    result.pml4e = (virtual & 0x1FF);
    return result; 
}

void vmm_map(pool_allocator_t* frame_allocator, uintptr_t* pml4, uintptr_t phys, uintptr_t virt, uint16_t pcid, uint64_t flags)
{
    if(!pml4)
        return;

    pml4 = HH_ADDR(pml4);

    if((phys % PMM_PAGE_SIZE != 0) || (virt % PMM_PAGE_SIZE != 0))
        return;

    struct page_index index = get_page_indices(virt);
    const uintptr_t hhdm = get_hhdm();

    if(!(pml4[index.pml4e] & VMM_FLAG_PRESENT))
    {
        pml4[index.pml4e] = (uintptr_t)fetch_zero_pool(frame_allocator) | flags | VMM_FLAG_PRESENT;
    }

    uint64_t* pdpt = ENTRY_ADDR(pml4[index.pml4e]);

    if(!(pdpt[index.pdpte] & VMM_FLAG_PRESENT))
    {
        pdpt[index.pdpte] = (uintptr_t)fetch_zero_pool(frame_allocator) | flags | VMM_FLAG_PRESENT;
    }

    uint64_t* pd = ENTRY_ADDR(pdpt[index.pdpte]);

    if(!(pd[index.pde] & VMM_FLAG_PRESENT))
    {
        pd[index.pde] = (uintptr_t)fetch_zero_pool(frame_allocator) | flags | VMM_FLAG_PRESENT;
    }

    uint64_t* pt = ENTRY_ADDR(pd[index.pde]);

    //handle already mapped page
    if(pt[index.pte] & VMM_FLAG_PRESENT)
    { return; }

    pt[index.pte] = phys | flags | VMM_FLAG_PRESENT;
}

void vmm_create_kernel_memmap(pool_allocator_t* frame_allocator, uintptr_t** pml4)
{
    const uintptr_t kernel_length = (uintptr_t)(&kaddr_end) - (uintptr_t)(&kaddr_start);
    const uintptr_t hhdm = get_hhdm();

    uint64_t* pml4_alloc = fetch_zero_pool(frame_allocator);
    *pml4 = pml4_alloc;

    struct limine_memmap_entry* entry;
    for(uint64_t i = 0; i < mmap_request.response->entry_count; i++)
    {
        entry = mmap_request.response->entries[i];

        for(uintptr_t addr = ALIGN_DOWN(entry->base, PMM_PAGE_SIZE); addr < (entry->base + entry->length); addr += PMM_PAGE_SIZE)
        {
            vmm_map(frame_allocator, pml4_alloc, addr, addr + hhdm, 0, VMM_FLAG_READ_WRITE | VMM_FLAG_SUPERUSER_PAGE | VMM_FLAG_GLOBAL);
        }
    }

    uintptr_t kernel_phys = ALIGN_DOWN(kernel_addr_request.response->physical_base, PMM_PAGE_SIZE);
    uintptr_t kernel_virt = kernel_addr_request.response->virtual_base;
    for(uintptr_t offset = 0x0; offset < kernel_length; offset += PMM_PAGE_SIZE)
    {
        vmm_map(frame_allocator, pml4_alloc, kernel_phys + offset, kernel_virt + offset, 0, VMM_FLAG_READ_WRITE | VMM_FLAG_SUPERUSER_PAGE | VMM_FLAG_GLOBAL);
    }
}

void vmm_create_memmap(pool_allocator_t* frame_allocator, uint16_t pcid, uintptr_t** pml4)
{
    uint64_t* pml4_alloc = fetch_zero_pool(frame_allocator);
    if(pcid_supported)
    {
        *pml4 = (uintptr_t)pml4_alloc | (pcid & 0xFFF);
    }
    else
    {
        *pml4 = pml4_alloc;
    }

    pml4_alloc = HH_ADDR(pml4_alloc);

    for(uint32_t i = 256; i < 512; i++)
    {
        pml4_alloc[i] = vmm_pml_kernel[i];
    }
}

void vmm_load_memmap(uintptr_t* pml4)
{
    __asm__ volatile ("mov %0, %%cr3" :: "r"(pml4));
}

uintptr_t vmm_read_memmap()
{
    uint64_t addr;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(addr));
    return addr;
}

void vmm_invalidate(vmm_invalidate_desc_t desc)
{
    if(pcid_supported)
    {
        uint64_t mode = (desc.addr == 0) ? 1 : 0;
        __asm__ volatile ("invpcid %0, %1" :: "m"(desc), "r"(mode) : "memory");
        return;
    }
    
    if(desc.addr == 0)
        __builtin_trap();

    __asm__ volatile ("invlpg (%0)" :: "r"(desc.addr) : "memory");
}

uintptr_t get_hhdm()
{
    return hhdm_request.response->offset;
}

uintptr_t get_kernel_offset()
{
    return kernel_addr_request.response->virtual_base;
}

void init_virtual_memory()
{
    if(CPU_CAPABILITY_PCID_PRESENT && CPU_CAPABILITY_INVPCID_PRESENT)
    {
        uint64_t cr4;
        __asm__ volatile("mov %%cr4, %%rax" : "=a"(cr4));
        cr4 |= (1 << 17);
        __asm__ volatile("mov %0, %%cr4" :: "r"(cr4));
        pcid_supported = 1;
    }

    pool_allocator_t* alloc = acquire_pool_allocator(PMM_PAGE_SIZE, 2048 * PMM_PAGE_SIZE);
    vmm_create_kernel_memmap(alloc, &vmm_pml_kernel);
    vmm_load_memmap(vmm_pml_kernel);
}

uintptr_t vmm_alloc(pool_allocator_t* alloc, uintptr_t* pml4, uint16_t pcid, uintptr_t virtual_addr)
{
    virtual_addr = ALIGN_DOWN(virtual_addr, PMM_PAGE_SIZE);
    uintptr_t ptr = (uintptr_t)fetch_zero_pool(alloc);
    vmm_map(alloc, pml4, ptr, virtual_addr, pcid, VMM_FLAG_READ_WRITE);
    vmm_invalidate_desc_t desc = 
    {
        .addr = virtual_addr,
        .pcid = pcid
    };
    vmm_invalidate(desc);
    return virtual_addr;
}