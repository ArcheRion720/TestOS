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
    for(;;)
    {
        warn("A");
    }
}

void task_B()
{
    for(;;)
    { 
        log("B");
    }
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

    // fs->list(cstr("/"), fs);
    // fs->list(cstr("/lost+found/"), fs);
    // fs->list(cstr("/lost+found/../"), fs);
    // fs->list(cstr("/test/"), fs);

    // log("Test");
    // warn("Test 2");

    // page_table_t* test = malloc_page();
    // memset(test, 0, sizeof(page_table_t));

    // for(uint64_t i = 0; i < 500; i++)
    // {
    //     map_virt_memory(test, 0x100000 * i, (0x100000 * i) + 0x1000);
    // }

    // printf("CR3: %ixq\n", (uintptr_t)test);
    // __asm__ ("mov %0, %%cr3" :: "r"(test));

   // printf("CR3: %ixq\n", (uintptr_t)page_table);

    //map_virt_memory(page_table, 0x69000, 0x42000);

    // __asm__ ("mov %cr3,%rax");
    // __asm__ ("mov %rax,%cr3");


    string_t a_str = cstr("A");
    string_t b_str = cstr("B"); 

    start_task(a_str, &task_A, read_cr3(), false);
    start_task(b_str, &task_B, read_cr3(), true);

    start_tasking();

    for(;;)
    {
        asm("hlt");
    }
}

