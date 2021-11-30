#pragma once
#include <stdint.h>
#include "debug.h"

struct idtr_descriptor
{
    uint16_t offset015;
    uint16_t selector;
    uint8_t zero;
    uint8_t type;
    uint16_t offset1631;
    uint32_t offset3263;
    uint32_t zerohigh;
} __attribute__((packed));
typedef struct idtr_descriptor idtr_descriptor_t;

struct idtr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
typedef struct idtr idtr_t;

void init_intdt();
void register_intdt(uint32_t code, uint64_t addr);
void interrupt_handler();