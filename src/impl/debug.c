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

#define FORMAT_TIME_2(index, var)   \
    temp = var / 10;                \
    buffer[index] = '0' + temp;     \
    buffer[index + 1] = '0' + (var - 10 * temp);


void format_time(time_t time, char* buffer)
{
    uint8_t temp;
    //dd:mm:yyyy hh:mm:ss 
    FORMAT_TIME_2(0, time.day_month);
    buffer[2] = ':';
    FORMAT_TIME_2(3, time.month);
    buffer[5] = ':';
    buffer[6] = '2';
    buffer[7] = '0';
    FORMAT_TIME_2(8, time.year);
    buffer[10] = ' ';
    FORMAT_TIME_2(11, time.hour);
    buffer[13] = ':';
    FORMAT_TIME_2(14, time.minute);
    buffer[16] = ':';
    FORMAT_TIME_2(17, time.second);
    buffer[19] = '\0';
}

#undef FORMAT_TIME_2

void debug_time(time_t time)
{
    format_time(time, handy_buffer);
    term_ptr(handy_buffer, 19);
}