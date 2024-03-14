#include "utils.h"
#include "rtc.h"
#include <stdarg.h>

void str_cpy(uint8_t* src, uint8_t* dest, uint32_t count)
{
    for(uint32_t i = 0; i < count; i++)
    {
        dest[i] = src[i];
    }
}

void memset(uint8_t* addr, uint8_t value, uint64_t count)
{
    for(uint64_t i = 0; i < count; i++)
    {
        addr[i] = value;
    }
}

void memcpy(const uint8_t* src, uint8_t* dest, uint64_t count)
{
    while(count--)
    {
        *dest++ = *src++;
    }
}

void memmove(const uint8_t* src, uint8_t* dest, uint64_t count)
{
    if(src > dest)
    {
        memcpy(src, dest, count);
        return;
    }

    for(uint64_t i = count; i --> 0;)
    {
        dest[i] = src[i];
    }
}