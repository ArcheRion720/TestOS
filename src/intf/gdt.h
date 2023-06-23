#pragma once
#include <stdint.h>

#define GDT_NULL        0
#define GDT_CS0_64      1
#define GDT_DS0_64      2
#define GDT_CS3_64      3
#define GDT_DS3_64      4
#define GDT_CS0_32      5
#define GDT_DS0_32      6
#define GDT_CS0_16      7
#define GDT_DS0_16      8
#define GDT_TSS         9

#define GDT_SEGMENT_OFFSET(x) (x * sizeof(gdtr_descriptor_t))

#define GDT_PRESENT     (1 << 7)
#define GDT_DPL_USER    ((1 << 5) | (1 << 6))
#define GDT_CODE_DATA   (1 << 4)
#define GDT_EXEC        (1 << 3)
#define GDT_DIR_DOWN    (1 << 2)
#define GDT_READ_WRITE  (1 << 1)

#define GDT_FL_GRAN     (1 << 7)
#define GDT_FL_32SEG    (1 << 6)
#define GDT_FL_LONG     (1 << 5)

#define GDT_KERNEL_BASE (GDT_PRESENT | GDT_CODE_DATA | GDT_READ_WRITE)

struct gdtr_descriptor
{
    uint16_t limit015;
    uint16_t base015;
    uint8_t base1623;
    uint8_t access;
    uint8_t flags;
    uint8_t base2431;
} __attribute__((packed));
typedef struct gdtr_descriptor gdtr_descriptor_t;

struct tss_descriptor
{
    struct gdtr_descriptor low_bits;
    uint32_t base3263;
    uint32_t reserved;
} __attribute__((packed));
typedef struct tss_descriptor tss_descriptor_t;

struct gdtr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
typedef struct gdtr gdtr_t;

struct tss
{
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
	uint64_t ist1;
	uint64_t ist2;
	uint64_t ist3;
	uint64_t ist4;
	uint64_t ist5;
	uint64_t ist6;
	uint64_t ist7;
    uint64_t reserved2;
	uint16_t reserved3;
	uint16_t iopb;
} __attribute__((packed));
typedef struct tss tss_t;

void init_gdt();
void register_gdt(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);