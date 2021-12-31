#include "storage/ahci.h"
#include "storage/drive.h"
#include "storage/gpt.h"

drive_t drives[32];
uint8_t drive_count; 

drive_t* get_drive(uint8_t index)
{
    if(index >= drive_count)
        return 0;

    return &drives[index];
}

uint8_t get_drive_count()
{
    return drive_count;
}

void register_partition(drive_t* drive, partition_t partition)
{
    drive->partitions[drive->partition_count++] = partition;
    //TODO: detect filesystem
}

drive_t* register_drive()
{
    return &drives[drive_count++];
}

void find_partitions_mbr(drive_t* drive)
{
    drive->read(drive, 0, 1, drive->buffer);
    mbr_header_t* mbr = (mbr_header_t*)drive->buffer;

    for(int i = 0; i < 4; i++)
    {
        if(mbr->partitions[i].flags & 0x80)
        {
            partition_t part;
            part.flags |= 1;
            part.lba_start = mbr->partitions[i].lba_addr;
            part.lba_end = mbr->partitions[i].lba_addr + mbr->partitions[i].sectors;
            part.drive = drive;
            str_cpy("Partition MBR\0", &part.label, 14);
            register_partition(drive, part);
        }
    }
}

void find_partitions_gpt(drive_t* drive)
{
    drive->read(drive, 0x0, 2, drive->buffer);
    gpt_t* gpt = (gpt_t*)drive->buffer;

    if(gpt->gpt.signature != 0x5452415020494645)
        return;

    uint32_t entries_per_read = 512 / gpt->gpt.entry_size;
    uint32_t sectors_to_read = gpt->gpt.partitions_count / entries_per_read;

    for(uint8_t block = 0; block < sectors_to_read; block++)
    {
        drive->read(drive, 0x2 + block, 1, drive->buffer);
        for(uint8_t part = 0; part < entries_per_read; part++)
        {
            gpt_partition_entry_t gpt_partition = ((gpt_partition_entry_t*)drive->buffer)[part];
            partition_t partition;
            if(gpt_partition.type.low || gpt_partition.type.high)
            {
                partition.flags |= PART_PRESENT;
                if(gpt_partition.attributes & 1)
                    partition.flags |= PART_RESERVED;

                partition.lba_start = gpt_partition.lba_start;
                partition.lba_end = gpt_partition.lba_end;
                partition.drive = drive;
                str_cpy("Partition GPT\0", &partition.label, 14);
                register_partition(drive, partition);
            }
        }
    }
}

void find_partitions(drive_t* drive)
{
    if(!(drive->flags & DRIVE_DEV_PRESENT))
        return;

    if(!(drive->flags & DRIVE_DEV_PART_TABLE))
        return;

    if(drive->flags & DRIVE_DEV_GPT)
    {
        find_partitions_gpt(drive);
        return;
    }

    find_partitions_mbr(drive);
}