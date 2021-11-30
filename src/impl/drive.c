#include "drive.h"

disk_info_t* disks;
uint8_t disk_count;

void init_disks()
{
    disks = (disk_info_t*)alloc_block_adjacent((sizeof(disk_info_t) * 256) / 0x1000);
    init_ahci();
}

uint8_t get_disk_count()
{
    return disk_count;
}

void register_disk(disk_info_t disk)
{
    disks[disk_count] = disk;
    disk_count++;
}

disk_info_t* get_disk(uint8_t index)
{
    return &disks[index];
}

uint8_t read_disk(uint8_t id, uint64_t lba, uint64_t count, uint8_t* buffer)
{
    if(id > disk_count)
        return 1;
    
    disk_info_t* disk = &disks[id];
    if(!(disk->flags & 1))
        return 2;

    return disk->read_disk(disk, lba, count, buffer);
}

uint8_t write_disk(uint8_t id, uint64_t lba, uint64_t count, uint8_t* buffer)
{
    if(id > disk_count)
        return 1;
    
    disk_info_t* disk = &disks[id];
    if(!(disk->flags & 1))
        return 2;

    return disk->write_disk(disk, lba, count, buffer);
}