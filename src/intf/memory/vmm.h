#pragma once
#include <stdint.h>

#define PAGE_PRESENT            0x001
#define PAGE_READ_WRITE         0x002
#define PAGE_SUPERUSER          0x004
#define PAGE WRITE_THROUGH      0x008
#define PAGE_CACHE_DISABLED     0x010
#define PAGE_ACCESSED           0x020
#define PAGE_LARGE              0x080
#define PAGE_UNUSED0            0x200
#define PAGE_UNUSED1            0x400
#define PAGE_UNUSED2            0x800
#define PAGE_NO_EXECUTE         1 << 63

#define VIRT_ADDR_MASK          0x1FF
#define PHYS(x)                 ((x) - 0xffff800000000000)
#define HIGHER_HALF(x)          ((x) + 0xffff800000000000)

#define SETUP_PAGE_LEVEL(TABLE, INDEX, CURRENT_TABLE)                               \
    page_directory_entry = CURRENT_TABLE->entries[INDEX];                           \
    page_table_t* TABLE;                                                            \
    if(!(page_directory_entry & PAGE_PRESENT))                                      \
    {                                                                               \
        TABLE = (page_table_t*)alloc_block_single();                                \
        set_page_address(&page_directory_entry, PHYS((uintptr_t)TABLE));            \
        page_directory_entry |= (PAGE_PRESENT | PAGE_READ_WRITE);                   \
        CURRENT_TABLE->entries[INDEX] = page_directory_entry;                       \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        TABLE = (page_table_t*)get_page_address(&page_directory_entry);             \
    }

struct page_table_index
{
    uintptr_t page_directory_pointer_index;
    uintptr_t page_directory_index;
    uintptr_t page_table_index;
    uintptr_t page_index;
};

typedef struct page_table_index page_table_index_t;
typedef uint64_t page_table_directory_t;

void set_page_address(page_table_directory_t* page, uintptr_t physical_addr);
uintptr_t get_page_address(page_table_directory_t* page);

struct page_table 
{
    page_table_directory_t entries[512];
} __attribute__((aligned(0x1000)));

typedef struct page_table page_table_t;

void init_virtual_memory_manager();
void map_memory(uintptr_t virtual_addr, uintptr_t physical_addr);
void set_level4_page_table(page_table_t* pml4);
page_table_index_t locate_addr(uintptr_t virtualAddress);
page_table_t* create_kernel_mapped_table();