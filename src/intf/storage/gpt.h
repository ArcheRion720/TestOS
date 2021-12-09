#pragma once 
#include "storage/mbr.h"

struct uint128 
{
    uint64_t low;
    uint64_t high;
};
typedef struct uint128 uint128_t;

struct gpt_header
{
    uint64_t signature;
    uint32_t revision;
    uint32_t header_size;
    uint32_t CRC32;
    uint32_t reserved;
    uint64_t lba_addr_header;
    uint64_t lba_addr_alt_header;
    uint64_t lba_first_entry;
    uint64_t lba_last_entry;
    uint128_t guid;
    uint64_t lba_partition_entries;
    uint32_t partitions_count;
    uint32_t entry_size;
    uint32_t CRC32_partitions;
};
typedef struct gpt_header gpt_header_t;

struct gpt
{
    mbr_header_t pmbr;
    gpt_header_t gpt;
};
typedef struct gpt gpt_t;

struct gpt_partition_entry
{
    uint128_t type;
    uint128_t unique;
    uint64_t lba_start;
    uint64_t lba_end;
    uint64_t attributes;
    uint8_t label[72];
};
typedef struct gpt_partition_entry gpt_partition_entry_t;