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
    printf("Test6: %iu %iu", 10, 20);

    while(1)
    {
        asm("pause");
    }

    asm("hlt");
}