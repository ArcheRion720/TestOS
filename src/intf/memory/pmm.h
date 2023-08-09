#pragma once
#include <stdint.h>
#include <stddef.h>

#define PMM_PAGE_SIZE 0x1000
#define PMM_BLOCK_USED 1
#define PMM_BLOCK_FREE 0
#define PMM_SIZE_PAGES(x) ((uintptr_t)(x) >> 12)

struct memory_block
{
    uint32_t used : 1;
    uint32_t size : 31;
};

struct memory_area
{
    struct memory_area* next;
    uint32_t pages_total;
    uint32_t pages_used;
    uint32_t block_count;
    struct memory_block blocks[];
};
typedef struct memory_area* memory_area_ptr;

void* malloc(uint64_t size);
void free(void* ptr);
