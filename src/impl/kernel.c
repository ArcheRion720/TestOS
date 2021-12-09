#include "kernel.h"
#include "storage/gpt.h"
#include "fs/ext2.h"

void kernel_start(struct stivale2_struct* stivale)
{
    initialize(stivale);

    printf("Current time: %t\n", read_rtc_time());

    for(int i = 0; i < get_drive_count(); i++)
    {
        drive_t* drive = get_drive(i);
        printf("Drive %iu (%s):\n", i, drive->label);
        for(int p = 0; p < drive->partition_count; p++)
        {
            partition_t part = drive->partitions[p];
            if(part.flags & 1)
            {
                printf("\tPartition %iu: %s ", p, part.label);
                if(part.flags & 2)
                {
                    printf("(RESERVED)");
                }
                printf("\n");
            }
        }
    }

    drive_t* ext2_drive = get_drive(1);
    partition_t* ext2_part;
    for(int i = 0; i < ext2_drive->partition_count; i++)
    {
        if(ext2_drive->partitions[i].flags & PART_RESERVED)
            continue;

        ext2_part = &ext2_drive->partitions[i];
    }

    printf("Partition start: %ixq\n", ext2_part->lba_start);

    ext2_drive->read(ext2_drive, ext2_part->lba_start + 0x2, 2, ext2_drive->buffer);
    ext2_super_block_t* block = (ext2_super_block_t*)ext2_drive->buffer;

    printf("EXT2 signature: %ixw\n", block->signature);

    while(1)
    {
        asm("pause");
    }

    asm("hlt");
}