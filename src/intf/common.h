#pragma once
#include <stdint.h>

/*
    BITSET
*/

#define BIT_MASK(x)         (1 << ((x) % 8))
#define BIT_SLOT(x)         ((x) / 8)
#define SET_BIT(arr, x)     ((arr)[BIT_SLOT(x)] |= BIT_MASK(x))
#define CLEAR_BIT(arr, x)   ((arr)[BIT_SLOT(x)] &= ~BIT_MASK(x))
#define TEST_BIT(arr, x)    ((arr)[BIT_SLOT(x)] & BIT_MASK(x))
#define TOGGLE_BIT(arr, x)  ((arr)[BIT_SLOT(x)] ^ BIT_MASK(x))
#define BIT_SLOTS(n)        ((n + 7) / 8)

/*
    MEMORY
*/

#define PAGE_SIZE       0x1000
#define ALIGN_UP(x, al) ((x + (al - 1)) & ~(al-1))
#define HIGH(phys)      ((phys) + 0xffff800000000000)
#define PHYS(high)      ((high) - 0xffff800000000000)

/*
    BOOL
*/
typedef enum { false, true } bool_t;

/*
    STRUCTS
*/

struct registers
{
    uint64_t gs;
    uint64_t fs;
    uint64_t es;
    uint64_t ds;
    uint64_t cr2;
    uint64_t cr3;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t interrupt;
    uint64_t error;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t tss;
} __attribute__((packed));
typedef struct registers registers_t;