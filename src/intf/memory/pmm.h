#pragma once
#include <stdint.h>
#include "linked_list.h"

#define PMM_PAGE_SIZE 0x1000
#define PMM_BLOCKS_LIMIT ((PMM_PAGE_SIZE - sizeof(struct memory_area)) / (sizeof(struct memory_block)))
#define PMM_BLOCK_USED 1
#define PMM_BLOCK_FREE 0

#define PMM_POOL_INIT_SIZE (PMM_PAGE_SIZE * 16)

#define SIZE_IN_PAGES(x) ((x) >> 12)

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

struct pool_allocator
{
    struct link items;
    uint32_t item_size;
    uint16_t size;
    uint16_t used;
} __attribute__((aligned(8)));
typedef struct pool_allocator pool_allocator_t;

void init_memory_manager();
void* malloc(uint64_t size);
void free(void* ptr);

void pool_allocator_create(pool_allocator_t* allocator, uint32_t item_size, uint32_t mem_size);
pool_allocator_t* pool_allocator_acquire(uint32_t item_size, uint32_t mem_size);
void pool_allocator_dispose(pool_allocator_t* allocator);

void* pool_fetch(pool_allocator_t* alloc);
void* pool_fetch_zero(pool_allocator_t* alloc);
void pool_drop(pool_allocator_t* alloc, void* item);