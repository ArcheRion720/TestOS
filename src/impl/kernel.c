#include "kernel.h"
#include "storage/gpt.h"

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

    while(1)
    {
        asm("pause");
    }

    asm("hlt");
}