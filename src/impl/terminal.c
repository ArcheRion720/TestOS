#include "terminal.h"
#include "limine.h"
#include "print.h"

static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

static struct limine_terminal* terminal;
static limine_terminal_write term_write;

void init_terminal()
{
    if (terminal_request.response == NULL || terminal_request.response->terminal_count < 1) 
    {
        //TODO: PANIC
        return;
    }

    terminal = terminal_request.response->terminals[0];
    term_write = terminal_request.response->write;

    print_fmt("Initialised debug temrinal\n");
}

void terminal_write(const uint8_t* string, size_t length)
{
    term_write(terminal, (const char*)string, length);
}