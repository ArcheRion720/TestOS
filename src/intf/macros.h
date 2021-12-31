#pragma once

#define PAGE_SIZE       0x1000
#define ALIGN_UP(x, al) ((x + (al - 1)) & ~(al-1))
#define HIGH(phys)      ((phys) + 0xffff800000000000)
#define PHYS(high)      ((high) - 0xffff800000000000)