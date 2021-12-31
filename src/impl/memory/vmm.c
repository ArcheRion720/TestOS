#include "memory/vmm.h"
#include "memory/pmm.h"
#include "hal.h"
#include "utils.h"

page_table_directory_t kernel_mappings[256];
page_table_t* page_level4_table;

page_table_index_t locate_addr(uintptr_t virtual_addr)
{
    page_table_index_t result;

    virtual_addr >>= 12;
    result.page_index = virtual_addr & VIRT_ADDR_MASK;

    virtual_addr >>= 9;
    result.page_table_index = virtual_addr & VIRT_ADDR_MASK;

    virtual_addr >>= 9;
    result.page_directory_index = virtual_addr & VIRT_ADDR_MASK;

    virtual_addr >>= 9;
    result.page_directory_pointer_index = virtual_addr & VIRT_ADDR_MASK;

    return result;
}

void map_memory(uintptr_t virtual_addr, uintptr_t physical_addr)
{
    page_table_index_t index = locate_addr(virtual_addr);
    page_table_directory_t page_directory_entry;

    SETUP_PAGE_LEVEL(page_directory_pointer, index.page_directory_pointer_index, page_level4_table);
    SETUP_PAGE_LEVEL(page_directory, index.page_directory_index, page_directory_pointer);
    SETUP_PAGE_LEVEL(page_table, index.page_table_index, page_directory);

    page_directory_entry = page_table->entries[index.page_index];
    set_page_address(&page_directory_entry, physical_addr);
    page_directory_entry |= (PAGE_PRESENT | PAGE_READ_WRITE);

    page_table->entries[index.page_index] = page_directory_entry;
}

void set_level4_page_table(page_table_t* pml4)
{
    page_level4_table = pml4;

    uintptr_t addr = PHYS((uintptr_t)pml4);
    asm ("mov %0, %%cr3" :: "r"(addr) : "memory");
}

void set_page_address(page_table_directory_t* page, uintptr_t physical_addr)
{
    physical_addr &= 0x000000ffffffffff;
    *page &= 0xfff0000000000fff;
    *page |= (physical_addr << 12);
}

uintptr_t get_page_address(page_table_directory_t* page)
{
    return (*page & 0x000ffffffffff000) >> 12;
}

page_table_t* create_kernel_mapped_table()
{
    page_table_t* result = (page_table_t*)malloc_page();
    for(uint16_t i = 0; i < 256; i++)
    {
        result->entries[i + 256] = kernel_mappings[i];
    }
    return result;
}

void init_virtual_memory_manager()
{
    page_table_t* table = (page_table_t*)HIGH(read_cr3());
    for(uint16_t i = 0; i < 256; i++)
    {
        kernel_mappings[i] = table->entries[i + 256];
    }

    printf("Initialised virtual memory\n");
}