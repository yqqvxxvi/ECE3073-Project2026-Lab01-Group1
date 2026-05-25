#ifndef VGA_H
#define VGA_H

#include <stdint.h>

/* Change these if your system uses different names */
#ifndef VGA_PIXEL_BUF_BASE
#define VGA_PIXEL_BUF_BASE 0x08000000
#endif

#ifndef VGA_WIDTH
#define VGA_WIDTH 320
#endif

#ifndef VGA_HEIGHT
#define VGA_HEIGHT 240
#endif

/* 16-bit RGB565 colour definitions */
#define VGA_BLACK   0x0000
#define VGA_WHITE   0xFFFF
#define VGA_RED     0xF800
#define VGA_GREEN   0x07E0
#define VGA_BLUE    0x001F
#define VGA_YELLOW  0xFFE0
#define VGA_CYAN    0x07FF
#define VGA_MAGENTA 0xF81F

void VGA_plot_pixel(int x, int y, uint16_t colour);
void VGA_clear_screen(uint16_t colour);
void VGA_draw_hline(int x1, int x2, int y, uint16_t colour);
void VGA_draw_vline(int x, int y1, int y2, uint16_t colour);
void VGA_draw_rect(int x, int y, int width, int height, uint16_t colour);
void VGA_fill_rect(int x, int y, int width, int height, uint16_t colour);
void VGA_draw_line(int x0, int y0, int x1, int y1, uint16_t colour);

#endif
