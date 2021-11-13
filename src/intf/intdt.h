#pragma once
#include <stdint.h>

#pragma pack(push, 1)
typedef struct
{
    uint16_t offset015;
    uint16_t selector;
    uint8_t zero;
    uint8_t type;
    uint16_t offset1631;
    uint32_t offset3263;
    uint32_t zerohigh;
} idtr_descriptor_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    uint16_t limit;
    uint64_t base;
} idtr_t;
#pragma pack(pop)

void init_intdt();
void register_intdt(uint32_t code, uint64_t addr);
void interrupt_handler();