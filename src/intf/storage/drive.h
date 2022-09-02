#pragma once
#include <stdint.h>

#define DRIVE_DEV_PRESENT       1
#define DRIVE_DEV_PART_TABLE    (1 << 1)
#define DRIVE_DEV_GPT           (1 << 2)

#define PART_PRESENT    1
#define PART_RESERVED   (1 << 1)

#define FS_NONE 0
#define FS_EXT2 1

struct drive;
struct partition;

typedef uint8_t (*drive_io_function)(struct drive*, uint64_t, uint64_t, uint8_t*);
typedef uint8_t (*partition_function)(const char* directory);

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

    struct drive* drive;
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

    drive_io_function read;
    drive_io_function write;
    uint8_t* buffer;
};

typedef struct drive drive_t;

uint8_t get_drive_count();
drive_t* get_drive(uint8_t index);
drive_t* register_drive();
void register_partition(drive_t* drive, partition_t partition);
void find_partitions(drive_t* drive);