#include "terminal.h"
#include "utils.h"
// #include "sync.h"
#include "limine.h"

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

    // ticket_lock_init(PRINT_LOCK);
    printf_ll("Initialised debug temrinal\n");
}

void terminal_write(const char* string, size_t length)
{
    term_write(terminal, string, length);
}