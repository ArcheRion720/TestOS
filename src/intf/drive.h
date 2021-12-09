#pragma once

#include <stdint.h>
#include <stddef.h>
#include "memory/pmm.h"
#include "memory/vmm.h"
#include "utils.h"

struct disk_info
{
    uint8_t flags;
    uint32_t type;
    uint32_t identifier;

    char label[32];

    uint8_t (*read_disk)  (struct disk_info* disk, uint64_t lba, uint64_t sector, uint8_t* buffer);
    uint8_t (*write_disk) (struct disk_info* disk, uint64_t lba, uint64_t sector, uint8_t* buffer);
};
typedef struct disk_info disk_info_t;

uint8_t get_disk_count();
void register_disk(disk_info_t disk);
disk_info_t* get_disk(uint8_t index);
void init_disks();

uint8_t read_disk(uint8_t id, uint64_t lba, uint64_t count, uint8_t* buffer);
uint8_t write_disk(uint8_t id, uint64_t lba, uint64_t count, uint8_t* buffer);