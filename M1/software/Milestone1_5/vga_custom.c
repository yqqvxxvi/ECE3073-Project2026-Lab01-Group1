#include "vga_custom.h"
#include "system.h"

volatile unsigned int *ADDR_EXPORT  = (unsigned int *)PIXEL_DATA_BASE;
volatile unsigned int *PIXEL_EXPORT = (unsigned int *)WREN_BASE;
volatile unsigned int *WREN_EXPORT  = (unsigned int *)0x08009120;

void write_pixel(unsigned int addr, unsigned char color)
{
    *ADDR_EXPORT  = addr;
    *PIXEL_EXPORT = color & 0xF;
    *WREN_EXPORT  = 1;
    *WREN_EXPORT  = 0;
}

void draw_color_stripes(void)
{
    for (int y = 0; y < VGA_PIX_HEIGHT; y++) {
        for (int x = 0; x < VGA_PIX_WIDTH; x++) {
            unsigned int  addr  = y * VGA_PIX_WIDTH + x;
            unsigned char color = x / 20;
            write_pixel(addr, color);
        }
    }
}

void draw_vertical_stripes(void)
{
    for (int y = 0; y < VGA_PIX_HEIGHT; y++) {
        unsigned char color = y / 15;
        for (int x = 0; x < VGA_PIX_WIDTH; x++) {
            unsigned int addr = y * VGA_PIX_WIDTH + x;
            write_pixel(addr, color);
        }
    }
}
