#pragma once
#include <stdint.h>

typedef void (*fs_operation)(struct filesystem*);

struct filesystem
{
    uint8_t name[64];
    fs_operation open;
    fs_operation close;
};