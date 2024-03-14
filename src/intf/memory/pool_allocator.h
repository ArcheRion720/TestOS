#pragma once
#include <stdint.h>
#include <stddef.h>
#include "datastruct/linked_list.h"

struct pool_allocator
{
    struct link items;
    uint32_t item_size;
    uint16_t size;
    uint16_t used;
} __attribute__((aligned(8)));

void pool_allocator_create(struct pool_allocator* allocator, uint32_t item_size, uint32_t mem_size);
struct pool_allocator* pool_allocator_acquire(uint32_t item_size, uint32_t mem_size);
void pool_allocator_dispose(struct pool_allocator* allocator);

void* pool_fetch(struct pool_allocator* alloc);
void* pool_fetch_zero(struct pool_allocator* alloc);
void pool_drop(struct pool_allocator* alloc, void* item);