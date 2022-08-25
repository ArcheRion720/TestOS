#pragma once
#include <stdint.h>
#include "common.h"
#include "utils.h"

#define PAGE_PRESENT            (0x001)
#define PAGE_READ_WRITE         (0x002)
#define PAGE_SUPERUSER          (0x004)
#define PAGE_WRITE_THROUGH      (0x008)
#define PAGE_CACHE_DISABLED     (0x010)
#define PAGE_ACCESSED           (0x020)
#define PAGE_LARGE              (0x080)
#define PAGE_UNUSED0            (0x200)
#define PAGE_UNUSED1            (0x400)
#define PAGE_UNUSED2            (0x800)
#define PAGE_NO_EXECUTE         (1 << 63)

#define VIRT_ADDR_INDEX_MASK    (0x1FF)
#define BITS40_MASK             (0x000000ffffffffff)
#define VIRT_ADDR_MASK          (0x000ffffffffff000)
#define PAGE_GET_ADDRESS(x)     ((x & VIRT_ADDR_MASK) >> 12)

struct page_table_index
{
    uint16_t page_directory_pointer_index;
    uint16_t page_directory_index;
    uint16_t page_table_index;
    uint16_t page_index;
};
typedef struct page_table_index page_table_index_t;

typedef uintptr_t page_table_entry_t;

struct page_table
{
    page_table_entry_t entries[512];
} __attribute__((aligned(0x1000)));
typedef struct page_table page_table_t;

void init_virtual_memory_manager();
page_table_index_t locate_virt_addr(uintptr_t addr);
void map_virt_memory(page_table_t* table, uintptr_t physical_addr, uintptr_t virtual_addr);
void set_page_address(page_table_entry_t* entry, uintptr_t address);
 
// struct page_table_index
// {
//     uintptr_t page_directory_pointer_index;
//     uintptr_t page_directory_index;
//     uintptr_t page_table_index;
//     uintptr_t page_index;
// };

// typedef struct page_table_index page_table_index_t;
// typedef uint64_t page_table_entry_t;

// void set_page_address(page_table_entry_t* page, uintptr_t physical_addr);
// uintptr_t get_page_address(page_table_entry_t* page);

// struct page_table 
// {
//     page_table_entry_t entries[512];
// } __attribute__((aligned(0x1000)));

// typedef struct page_table page_table_t;

// void map_memory(uintptr_t virtual_addr, uintptr_t physical_addr);
// void set_level4_page_table(page_table_t* pml4);
// page_table_index_t locate_addr(uintptr_t virtualAddress);
// page_table_t* create_kernel_mapped_table();