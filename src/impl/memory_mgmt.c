#include "memory_mgmt.h"
#include "capabilities.h"
#include "utils.h"
#include "boot_request.h"
#include "print.h"

uintptr_t hhdm;
uintptr_t vmm_kernel_map[512];

static uint64_t usable_memory = 0;
extern memory_area_ptr memory_head;
extern uint8_t pcid_enabled;

extern struct limine_memmap_request mmap_request;
extern struct limine_hhdm_request hhdm_request;
extern struct limine_kernel_address_request kernel_addr_request;

extern struct pool_allocator pmm_allocators;
extern uintptr_t kernel_mappings[512];

extern uint8_t kaddr_end[], kaddr_start[];

void print_mem_entry(struct limine_memmap_entry* e)
{
    print_fmt("Base: {xlong}\tLength: {xlong}\tType: {long}", &e->base, &e->length, &e->type);
}

void pmm_init()
{
    register_print_format("mem", print_mem_entry);

    if(mmap_request.response == 0 || mmap_request.response->entry_count == 0)
        return;

    memory_area_ptr current = 0;
    memory_area_ptr previous = 0;

    struct limine_memmap_entry* entry;
    for(uint64_t i = 0; i < mmap_request.response->entry_count; i++)
    {
        entry = mmap_request.response->entries[i];

        print_fmt("{mem}\n", entry);

        if(entry->type != LIMINE_MEMMAP_USABLE)
            continue;

        if(PMM_SIZE_PAGES(entry->length) < 2)
            continue;

        current = (memory_area_ptr)entry->base;
        current->pages_total = PMM_SIZE_PAGES(entry->length) - 1;
        current->pages_used = 0;
        current->block_count = 1;
        current->blocks[0].size = current->pages_total;
        current->blocks[0].used = PMM_BLOCK_FREE;

        if(previous)
            previous->next = HH_ADDR(current);
        else
            memory_head = HH_ADDR(current);

        usable_memory += (current->pages_total * PMM_PAGE_SIZE);
        previous = current;
    }
}

void vmm_init()
{
    const uintptr_t kernel_length = ((uintptr_t)&kaddr_end) - ((uintptr_t)&kaddr_start);

    uintptr_t pml4_alloc = (uintptr_t)pool_fetch_zero(pmm_kernel_allocator);

    struct vmm_mapping_desc desc;
    desc.pml4 = pml4_alloc;
    desc.pcid = 0;
    desc.flags = VMM_FLAG_GLOBAL | VMM_FLAG_READ_WRITE;

    for(uintptr_t addr = 0x0; addr < (VMM_SIZE_GB * 16); addr += VMM_SIZE_GB)
    {
        desc.paddr = addr;
        desc.vaddr = HH_ADDR(addr);
        desc.size = VMM_SIZE_GB;

        vmm_map(desc, pmm_kernel_allocator);
    }

    uintptr_t kernel_paddr = ALIGN_DOWN(kernel_addr_request.response->physical_base, PMM_PAGE_SIZE);
    uintptr_t kernel_vaddr = kernel_addr_request.response->virtual_base;

    desc.size = VMM_SIZE_KB;
    for(uintptr_t offset = 0x0; offset < kernel_length; offset += PMM_PAGE_SIZE)
    {
        desc.paddr = kernel_paddr + offset;
        desc.vaddr = kernel_vaddr + offset;

        vmm_map(desc, pmm_kernel_allocator);
    }

    if(CPU_CAPABILITY_PCID_PRESENT && CPU_CAPABILITY_INVPCID_PRESENT)
    {
        uint64_t cr4;
        __asm__ volatile("mov %%cr4, %%rax" : "=a"(cr4));
        cr4 |= (1 << 17);
        __asm__ volatile("mov %0, %%cr4" :: "r"(cr4));
        pcid_enabled = 1;
    }
}

void init_memory_manager()
{
    hhdm = hhdm_request.response->offset;

    pmm_init();
    pool_allocator_create(&pmm_allocators, sizeof(struct pool_allocator), PMM_PAGE_SIZE);
    pmm_kernel_allocator = pool_allocator_acquire(PMM_PAGE_SIZE, 0xF00000);
    vmm_init();
}