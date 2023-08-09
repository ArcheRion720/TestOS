#include "memory_mgmt.h"
#include "utils.h"

struct pool_allocator* pmm_kernel_allocator;
struct pool_allocator pmm_allocators; 

void pool_allocator_create(struct pool_allocator* allocator, uint32_t item_size, uint32_t mem_size)
{
    void* block = HH_ADDR(malloc(mem_size));
    
    item_size = ALIGN_UP(item_size, 8);
    uint32_t items_count = mem_size / item_size;

    allocator->items.next = &allocator->items;
    allocator->items.prev = &allocator->items;

    for(uint32_t i = 0; i < items_count; i++)
    {
        struct link* item = (struct link*)((uintptr_t)block + (i * item_size));
        linked_list_add_forward(item, &allocator->items);
    }

    allocator->size = items_count;
    allocator->used = 0;
    allocator->item_size = item_size;
}

struct pool_allocator* pool_allocator_acquire(uint32_t item_size, uint32_t mem_size)
{
    struct pool_allocator* result = (struct pool_allocator*)HH_ADDR(pool_fetch(&pmm_allocators));

    if(result == 0)
        return 0;

    pool_allocator_create(result, item_size, mem_size);
    return result;
}

void pool_allocator_dispose(struct pool_allocator* allocator)
{
    pool_drop(&pmm_allocators, allocator);
}

void* pool_fetch(struct pool_allocator* alloc)
{
    if(alloc->used == alloc->size)
    {
        __builtin_trap();
    }

    struct link* it;
    linked_list_foreach_link(it, &alloc->items)
    {
        linked_list_remove(it);
        alloc->used++;
        return LH_ADDR(it);
    }

    __builtin_trap();
    return 0;
}

void* pool_fetch_zero(struct pool_allocator* alloc)
{
    void* ptr = pool_fetch(alloc);
    memset(HH_ADDR(ptr), 0, alloc->item_size);
    return ptr;
}

void pool_drop(struct pool_allocator* alloc, void* item)
{
    linked_list_add_forward(&item, &alloc->items);
}