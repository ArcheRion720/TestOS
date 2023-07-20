#pragma once
#include <stdint.h>

#define RING_BUFFER_POOL_SIZE 32

struct ringbuffer_meta
{
    uint64_t size;
    uint64_t last_write;
    uint64_t last_read;
    uint8_t* data;
};

struct ringbuffer_meta* ringbuffer_create(uint64_t size);
void ringbuffer_destroy(struct ringbuffer_meta* meta);
uint64_t ringbuffer_read(struct ringbuffer_meta* meta, uint8_t* buffer, uint64_t max_length);
uint64_t ringbuffer_write(struct ringbuffer_meta* meta, uint8_t item);
uint64_t ringbuffer_write_buffer(struct ringbuffer_meta* meta, uint8_t* buffer, uint64_t length);