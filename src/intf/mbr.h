#pragma once

#include <stdint.h>
#include <stddef.h>

struct mbr_partition_entry
{
    uint8_t flags;
    uint32_t addr_start : 32;
    uint8_t type;
    uint32_t addr_end : 32;
    uint32_t lba_addr;
    uint32_t sectors;
} __attribute__((packed));

typedef struct mbr_partition_entry mbr_partition_entry_t;

struct mbr_header
{
    uint8_t bootstrap[440];
    uint32_t unique_disk_id;
    uint16_t reserved;
    mbr_partition_entry_t partitions[4];
    uint16_t boot_signature;
};

typedef struct mbr_header mbr_header_t;