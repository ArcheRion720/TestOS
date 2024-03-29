#pragma once
#include <stdint.h>

#define REG_FLAG_IF     (1 << 9)
#define REG_FLAG_IOPL   ((1 << 12) | (1 << 13))

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