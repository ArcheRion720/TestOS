#pragma once
#include <stdint.h>

#define DRIVE_DEV_PRESENT       1
#define DRIVE_DEV_PART_TABLE    (1 << 1)
#define DRIVE_DEV_GPT           (1 << 2)

#define PART_PRESENT    1
#define PART_RESERVED   (1 << 1)

typedef uint8_t (*drive_function)(struct drive*, uint64_t, uint64_t, uint8_t*);

struct partition
{
    /*
        Bit - Description
         0  - Present
         1  - Reserved by firmware (GPT)
    */
    uint16_t flags;
    uint16_t file_system;
    uint64_t lba_start;
    uint64_t lba_end;
    char label[32];
};

typedef struct partition partition_t;

struct drive 
{
    /*
        Bit - Description
         0  - Present
         1  - Detected parition table
         2  - MBR (0)/GPT (1)
    */
    uint16_t flags;
    uint16_t type;
    uint32_t port;
    char label[32];
    uint8_t partition_count;
    partition_t partitions[32];

    drive_function read;
    drive_function write;
    uint8_t* buffer;
};

typedef struct drive drive_t;

uint8_t get_drive_count();
drive_t* get_drive(uint8_t index);
void register_drive(drive_t drive);
void register_partition(drive_t* drive, partition_t partition);
void find_partitions(drive_t* drive);