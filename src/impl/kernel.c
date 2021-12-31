#include "kernel.h"
#include "storage/gpt.h"
#include "fs/fs.h"
#include "fs/ext2.h"
#include "macros.h"

void kernel_start(struct stivale2_struct* stivale)
{
    initialize(stivale);

    printf("Current time: %t\n", read_rtc_time());

    //Discover all ext2 partitions on all drives
    for(int i = 0; i < get_drive_count(); i++)
    {
        drive_t* drive = get_drive(i);
        if(discover_ext2_fs(drive) > 0)
            printf("Found EXT2 partition(s)!\n");
    }

    if(get_fs_count() > 0)
    {
        printf("Reading root directory:\n");
        file_system_t* fs = get_filesystem(0);
        fs->read("/", fs);
    }

    while(1)
    {
        asm("pause");
    }

    asm("hlt");
}