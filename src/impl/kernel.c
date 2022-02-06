#include "kernel.h"
#include "storage/gpt.h"
#include "fs/fs.h"
#include "fs/ext2.h"
#include "common.h"
#include "string.h"

uint32_t a = 0;
uint32_t b = 0;

void task_A()
{
    for(;;);
}

void task_B()
{
    for(;;)
    { }
}

void kernel_start(struct stivale2_struct* stivale)
{
    initialize(stivale);

    printf("Current time: %t\n", read_rtc_time());

    //Discover all ext2 partitions on all drives
    // for(int i = 0; i < get_drive_count(); i++)
    // {
    //     drive_t* drive = get_drive(i);
    //     if(discover_ext2_fs(drive) > 0)
    //         printf("Found EXT2 partition(s)!\n");
    // }

    // file_system_t* fs = get_filesystem(0);
    
    // string_t str = cstr("abc");
    // string_split_t spl = splitstr(str, '/');

    // fs->list(cstr("/"), fs);
    // fs->list(cstr("/lost+found/"), fs);
    // fs->list(cstr("/lost+found/../"), fs);
    // fs->list(cstr("/test/"), fs);

    // log("Test");
    // warn("Test 2");

    string_t a_str = cstr("A");
    string_t b_str = cstr("B"); 

    __asm__("cli");
    start_task(a_str, &task_A, read_cr3());
    start_task(b_str, &task_B, read_cr3());
    __asm__("sti");

    for(;;)
    {
        asm("hlt");
    }
}

