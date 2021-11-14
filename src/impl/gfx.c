#include "gfx.h"
#include "debug.h"
#include "utils.h"

static color_t* framebuffer;
static uint16_t frame_pitch;
static uint16_t frame_width;

extern struct PSF_header consolas_font;

void init_graphics(struct stivale2_struct* stivale)
{
    //Setup framebuffer
    struct stivale2_struct_tag_framebuffer* framebuffer_data;
    framebuffer_data = stivale2_get_tag(stivale, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

    if(framebuffer_data == NULL)
    {
        asm ("hlt");
    }

    framebuffer = (struct color_t*)framebuffer_data->framebuffer_addr;
    frame_pitch = framebuffer_data->framebuffer_pitch;
    frame_width = framebuffer_data->framebuffer_width;

    if(consolas_font.magic[0] != PSF_MAGIC0 || consolas_font.magic[1] != PSF_MAGIC1)
    {
        debug_write(TEXT("Loading font failed\n"));
        asm("hlt");
    }

    debug_write(TEXT("Loading font succeded\n"));

    if(consolas_font.mode & 0x02)
    {
        debug_write(TEXT("Unicode Table included\n"));
    }

    debug_write(TEXT("Initialised graphics\n"));
}

void putchar(unsigned short int character, int cx, int cy, color_t foreground, color_t background)
{
    uint8_t* bitmap = (uint8_t*)((uintptr_t)(&consolas_font) + (uintptr_t)sizeof(struct PSF_header));
    bitmap += consolas_font.charsize * character;

    for(int i = 0; i < consolas_font.charsize; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            framebuffer[(cx + (8 - j)) + ((i + cy) * frame_width)] = (*bitmap & (1 << j)) ? foreground : background;
        }

        bitmap++;
    }
}