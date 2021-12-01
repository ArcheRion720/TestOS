#include "kernel.h"

void kernel_start(struct stivale2_struct* stivale)
{
    initialize(stivale);

    printf("Current time: %t\n", read_rtc_time());
    printf("Test1: %iu\n", 423423);
    printf("Test2: %ixb\n", 423423);
    printf("Test3: %ixw\n", 423423);
    printf("Test4: %ixd\n", 423423);
    printf("Test5: %ixq\n", 423423);
    printf("Test6: %iu %iu\n", 10, 20);

    drive_t* drive = get_drive(0);
    printf("Drive 0: %s\n", drive->label);

    uint8_t testBuf[512];

    if(drive->read(drive, 0x0, 1, testBuf))
    {
        for(int i = 0; i < 512; i++)
        {
            printf("%ixb", testBuf[i]);
        }
    }
    
    while(1)
    {
        asm("pause");
    }

    asm("hlt");
}