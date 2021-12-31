#pragma once
#include "bootloader.h"

#define PAGE_SIZE       0x1000
#define PMM_HBLOCK_SIZE 0x2000
#define PMM_LOWEST_ORD  0x7
#define PMM_ENTRY_BITS  ((1 << (PMM_LOWEST_ORD + 1)) - 1)

struct memory_region
{
    uint64_t base;
    uint64_t alloc_base;
    uint64_t blocks;
    uint64_t bmap_reserved_blocks;
    uint64_t offsets[8];
};
typedef struct memory_region memory_region_t;

struct pmm_bitmap_iterator
{
    uint8_t valid;
    uint16_t region;
    uint64_t abs_bit;
    uint64_t rel_bit;
};
typedef struct pmm_bitmap_iterator pmm_bitmap_iterator_t;

struct malloc_report
{
    void* data;
    uint64_t size;
};
typedef struct malloc_report malloc_report_t;

void* malloc(uint64_t size);
void free(void* ptr, uint64_t size);
void* malloc_page();
void free_page(void* ptr);

malloc_report_t malloc_ex(uint64_t size);

void init_memory_manager(struct stivale2_struct* stivale);