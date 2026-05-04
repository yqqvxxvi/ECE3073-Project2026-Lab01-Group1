#include <stdio.h>
#include <stdint.h>
#include <io.h>
#include <system.h>
#include "sys/alt_stdio.h"

#include "image_data.h"

#define VGA_WIDTH        320
#define VGA_HEIGHT       240
#define VGA_PIXEL_COUNT  (VGA_WIDTH * VGA_HEIGHT)

#define SDRAM_FRAME_OFFSET 0x00100000
static volatile uint8_t * const SDRAM_FRAME =
    (volatile uint8_t *)(NEW_SDRAM_CONTROLLER_0_BASE + SDRAM_FRAME_OFFSET);

/*
   5x7 font lookup table.
   Each byte is one row.
   Only lower 5 bits are used.

   Example:
   0x0E = 01110
   0x11 = 10001
*/



void load_image_to_sdram(void)
{
    for (unsigned int i = 0; i < VGA_PIXEL_COUNT; i++)
    {
        SDRAM_FRAME[i] = image_data[i] & 0xF;
    }
}

const uint8_t font5x7[27][7] =
{
    /* SPACE */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* A */
    {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11},

    /* B */
    {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E},

    /* C */
    {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E},

    /* D */
    {0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E},

    /* E */
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F},

    /* F */
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10},

    /* G */
    {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0E},

    /* H */
    {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11},

    /* I */
    {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E},

    /* J */
    {0x01, 0x01, 0x01, 0x01, 0x11, 0x11, 0x0E},

    /* K */
    {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11},

    /* L */
    {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F},

    /* M */
    {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11},

    /* N */
    {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11},

    /* O */
    {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},

    /* P */
    {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10},

    /* Q */
    {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D},

    /* R */
    {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11},

    /* S */
    {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E},

    /* T */
    {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},

    /* U */
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},

    /* V */
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04},

    /* W */
    {0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A},

    /* X */
    {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11},

    /* Y */
    {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04},

    /* Z */
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F}
};

void write_pixel(unsigned int addr, unsigned char color)
{
    IOWR_32DIRECT(IMG_ADDRESS_BASE, 0, addr);
    IOWR_32DIRECT(PIXEL_DATA_BASE,  0, color & 0xF);



    IOWR_32DIRECT(WREN_BASE, 0, 1);

    IOWR_32DIRECT(WREN_BASE, 0, 0);
}

void draw_color_stripes(void)
{
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            unsigned int addr = y * VGA_WIDTH + x;
            unsigned char color = x / 20;

            write_pixel(addr, color);
        }
    }
}

void draw_vertical_stripes(void)
{
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        unsigned char color = y / 15;

        for (int x = 0; x < VGA_WIDTH; x++)
        {
            unsigned int addr = y * VGA_WIDTH + x;

            write_pixel(addr, color);
        }
    }
}

void clear_sdram_image(unsigned char color)
{
    for (unsigned int i = 0; i < VGA_PIXEL_COUNT; i++)
    {
        SDRAM_FRAME[i] = color & 0xF;
    }
}

void display_sdram_image(void)
{
    for (unsigned int addr = 0; addr < VGA_PIXEL_COUNT; addr++)
    {
        unsigned char color = SDRAM_FRAME[addr] & 0xF;
        write_pixel(addr, color);
    }
}

void store_image_to_sdram(void)
{
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            unsigned int addr = y * VGA_WIDTH + x;
            unsigned char color;

            /* 1. Border */
            if (x < 5 || x >= VGA_WIDTH - 5 || y < 5 || y >= VGA_HEIGHT - 5)
            {
                color = 0xF;   // bright border
            }

            /* 2. Center rectangle */
            else if (x >= 110 && x < 210 && y >= 70 && y < 170)
            {
                color = 0xC;   // center box
            }

            /* 3. Diagonal cross line */
            else if ((x / 2) == y || (x / 2) == (VGA_HEIGHT - 1 - y))
            {
                color = 0xE;   // diagonal lines
            }

            /* 4. Background gradient */
            else
            {
                color = ((x / 20) + (y / 15)) & 0xF;
            }

            SDRAM_FRAME[addr] = color & 0xF;
        }
    }
}

static void busy_delay(volatile int count)
{
    for (volatile int i = 0; i < count; i++);
}

void sdram_set_pixel(int x, int y, unsigned char color)
{
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT)
        return;

    unsigned int addr = y * VGA_WIDTH + x;
    SDRAM_FRAME[addr] = color & 0xF;
}

int get_font_index(char c)
{
    if (c == ' ')
        return 0;

    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 1;

    if (c >= 'a' && c <= 'z')
        return c - 'a' + 1;

    return 0;
}

void draw_char_to_sdram(int x, int y, char c, unsigned char color, int scale)
{
    int index = get_font_index(c);

    for (int row = 0; row < 7; row++)
    {
        uint8_t row_data = font5x7[index][row];

        for (int col = 0; col < 5; col++)
        {
            int bit = (row_data >> (4 - col)) & 0x01;

            if (bit)
            {
                for (int sy = 0; sy < scale; sy++)
                {
                    for (int sx = 0; sx < scale; sx++)
                    {
                        sdram_set_pixel(
                            x + col * scale + sx,
                            y + row * scale + sy,
                            color
                        );
                    }
                }
            }
        }
    }
}

void draw_string_to_sdram(int x, int y, const char *text, unsigned char color, int scale)
{
    int cursor_x = x;
    int cursor_y = y;

    int char_width = 5 * scale;
    int char_height = 7 * scale;
    int spacing = 1 * scale;

    while (*text != '\0')
    {
        if (*text == '\n')
        {
            cursor_x = x;
            cursor_y += char_height + spacing;
        }
        else
        {
            draw_char_to_sdram(cursor_x, cursor_y, *text, color, scale);
            cursor_x += char_width + spacing;
        }

        text++;
    }
}

void generate_text_image_to_sdram(void)
{
    clear_sdram_image(0x0);

    /* Background pattern */
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            unsigned char bg = ((x / 40) + (y / 30)) & 0x3;
            sdram_set_pixel(x, y, bg);
        }
    }

    /* Border */
    for (int x = 0; x < VGA_WIDTH; x++)
    {
        sdram_set_pixel(x, 0, 0xF);
        sdram_set_pixel(x, VGA_HEIGHT - 1, 0xF);
    }

    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        sdram_set_pixel(0, y, 0xF);
        sdram_set_pixel(VGA_WIDTH - 1, y, 0xF);
    }

    /* Text */
    draw_string_to_sdram(30, 60, "HELLO WHOEVER IS READING THIS", 0xF, 1);
    draw_string_to_sdram(30, 95, "LEMME TELL U", 0xE, 2);
    draw_string_to_sdram(30, 125, "ZIHANG IS GAY", 0xC, 2);
}


int main(void)
{
    alt_putstr("VGA SDRAM text image test starting\n");

    IOWR_32DIRECT(WREN_BASE, 0, 0);

//    generate_text_image_to_sdram();

    alt_putstr("Image generated and stored into SDRAM\n");

    load_image_to_sdram();
    while (1) { display_sdram_image(); }

    return 0;
}
