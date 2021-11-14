#include "debug.h"

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
    term_ptr(TEXT("Initialised debug temrinal\n"));
}

void debug_write(const char* string, size_t length)
{
    term_ptr(string, length);
}

void debug_number(uint64_t num)
{
    itoah(num, handy_buffer);
    term_ptr(handy_buffer, 16);
}

void debug_newl()
{
    term_ptr("\n", 1);
}