#pragma once
#include "bootloader.h"

#define PAGE_SIZE 0x1000
#define HBLOCK_SIZE 0x2000
#define LOWEST_ORDER 0x7
#define HIGHER_HALF(x) ((x) + 0xffff800000000000)

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

void* malloc(uint64_t size);
void free(void* ptr, uint64_t size);
void* malloc_page();
void free_page(void* ptr);

void init_memory_manager(struct stivale2_struct* stivale);