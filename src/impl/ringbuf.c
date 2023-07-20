#include "ringbuf.h"
#include "memory/pmm.h"
#include "memory/vmm.h"

static pool_allocator_t* ringbuffer_alloc = 0; 

struct ringbuffer_meta* ringbuffer_create(uint64_t size)
{
    if(ringbuffer_alloc == 0)
    {
        ringbuffer_alloc = pool_allocator_acquire(sizeof(struct ringbuffer_meta), sizeof(struct ringbuffer_meta) * RING_BUFFER_POOL_SIZE);
    }

    struct ringbuffer_meta* buffer = pool_fetch(ringbuffer_alloc);
    if(!buffer)
        return 0;

    buffer = HH_ADDR(buffer);

    uintptr_t alloc = malloc(size);
    if(!alloc)
    {
        pool_drop(ringbuffer_alloc, buffer);
        return 0;
    }

    alloc = HH_ADDR(alloc);

    buffer->data = alloc;
    buffer->last_read = 0;
    buffer->last_write = 0;
    buffer->size = size;

    return buffer;
}

void ringbuffer_destroy(struct ringbuffer_meta* meta)
{
    free(meta->data);
    pool_drop(ringbuffer_alloc, meta);
}

uint64_t ringbuffer_read(struct ringbuffer_meta* meta, uint8_t* buffer, uint64_t max_length)
{
    uint64_t read = 0;
    while(meta->last_read != meta->last_write && read < max_length)
    {
        if(meta->last_read >= meta->size)
            meta->last_read -= meta->size;
        
        *buffer++ = meta->data[meta->last_read++];
        read++;
    }

    return read;
}

uint64_t ringbuffer_write(struct ringbuffer_meta* meta, uint8_t item)
{
    return ringbuffer_write_buffer(meta, &item, 1);
}

uint64_t ringbuffer_write_buffer(struct ringbuffer_meta* meta, uint8_t* buffer, uint64_t length)
{
    uint64_t limit = 0;
    if(meta->last_read == 0)
        limit = meta->size - 1;
    else
        limit = meta->last_read - 1;

    uint64_t written = 0;
    while(meta->last_write != limit && written < length)
    {
        if(meta->last_write >= meta->size)
            meta->last_write -= meta->size;

        meta->data[meta->last_write++] = *buffer++;
        written++;
    }

    return written;
}