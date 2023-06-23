#include "ps2.h"
#include "hal.h"
#include "interrupts.h"
#include "utils.h"

uint8_t lastKey = 0;
uint8_t keyCache[256];
uint16_t index = 0;
uint8_t keyboard_status = 0;

void init_keyboard()
{
    register_isr_handler(33, (isr_t)&keyboard_handler);
    keyboard_status = 1;
    // log("Initialised PS2");
}

uint8_t keyboard_enabled()
{
    return keyboard_status;
}

void keyboard_read_key()
{
    lastKey = 0;
    if(inport8(0x64) & 1)
        lastKey = inport8(0x60);
}

char keyboard_get_key()
{
    if(index == 0)
        return '\0';

    index--;
    char result = keyCache[0];
    for(int i = 0; i < 255; i++)
    {
        keyCache[i] = keyCache[i + 1];
    }
    return result;
}

static char* qwerty = "qwertyuiop";
static char* asdfgh = "asdfghjkl";
static char* zxcvbn = "zxcvbnm";
static char* number = "123456789";

uint8_t keyboard_to_ascii(uint8_t key)
{
    if(key == 0x1C) return '\n';
    if(key == 0x39) return ' ';
    if(key == 0xE) return '\r';
    if(key == POINT_RELEASED) return '.';
    if(key == SLASH_RELEASED) return '/';
    if(key == ZERO_PRESSED) return '0';

    if(key >= ONE_PRESSED && key <= NINE_PRESSED)
        return number[key - ONE_PRESSED];

    else if(key >= 0x10 && key <= 0x1C)
        return qwerty[key - 0x10];

    else if(key >= 0x1E && key <= 0x26)
        return asdfgh[key - 0x1E];

    else if(key >= 0x2C && key <= 0x32)
        return zxcvbn[key - 0x2C];

    return 0;
}

void keyboard_handler(registers_t* regs)
{
    keyCache[index++] = keyboard_to_ascii(inport8(0x60));
}