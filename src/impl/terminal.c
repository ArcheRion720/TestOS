#include "terminal.h"
#include "utils.h"
#include "sync.h"

void (*term_ptr)(const char* string, size_t length);
char handy_buffer[128];

void init_terminal(struct stivale2_struct* stivale)
{
    struct stivale2_struct_tag_terminal* terminal_tag;
    terminal_tag = stivale2_get_tag(stivale, STIVALE2_STRUCT_TAG_TERMINAL_ID);

    if(terminal_tag == NULL)
    {
        asm ("hlt");
    }

    term_ptr = terminal_tag->term_write;
    ticket_lock_init(PRINT_LOCK);
    printf("Initialised debug temrinal\n");
}

void terminal_write(const char* string, size_t length)
{
    term_ptr(string, length);
}