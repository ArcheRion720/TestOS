#pragma once
#include <stdint.h>

#define IA32_EFER_SCE (1 << 0)

#define MSR_IA32_EFER   0xC0000080

#define MSR_SYSENTER_CS 0x174
#define MSR_SYSENTER_SP 0x175
#define MSR_SYSENTER_IP 0x176

// #define rdmsr(idx, low, high) __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(idx))
// #define wrmsr(idx, low, high) __asm__ volatile ("wrmsr" :: "c"(idx), "a"(low), "d"(high))

#define rdmsr(idx, value)                                                       \
    ({                                                                          \
        uint32_t low, high;                                                     \
        __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(idx));          \
        value = low | (high << 32);                                             \
    })

#define wrmsr(idx, value)                                                       \
    ({                                                                          \
        uint32_t low = (uint32_t)value;                                         \
        uint32_t high = (uint32_t)(value >> 32);                                \
        __asm__ volatile ("wrmsr" :: "c"(idx), "a"(low), "d"(high));            \
    })