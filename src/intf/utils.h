#pragma once
#include <stdint.h>
#include <stdarg.h>

#define ALIGN_UP(x, al) (((x) + (al - 1)) & ~(al-1))
#define ALIGN_DOWN(x, al) ((x / al) * al)

#define ROTL(arr, i, offset, len, type) \
    if(len > i) \
        memmove(((uint8_t*)(&arr[i + offset])), ((uint8_t*)(&arr[i])), ((len - i - offset) * sizeof(type)))

void memset(uint8_t* addr, uint8_t value, uint64_t count);
void memcpy(const uint8_t* src, uint8_t* dest, uint64_t count);
void memmove(const uint8_t* src, uint8_t* dst, uint64_t count);