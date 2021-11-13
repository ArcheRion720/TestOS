#pragma once
#include "bootloader.h"
#include "memory.h"
#include <limits.h>

#define PSF_MAGIC0  0x36
#define PSF_MAGIC1  0x04

typedef struct color
{
    uint8_t blue;
    uint8_t red;
    uint8_t green;
    uint8_t unused;
} color_t;

struct PSF_header
{
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charsize;
};

void init_graphics(struct stivale2_struct* stivale);
void putchar(unsigned short int character, int cx, int cy, color_t foreground, color_t background);