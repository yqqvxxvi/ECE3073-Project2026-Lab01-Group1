#include "vga.h"

static volatile uint16_t *pixel_buffer = (volatile uint16_t *)VGA_PIXEL_BUF_BASE;

void VGA_plot_pixel(int x, int y, uint16_t colour)
{
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT)
        return;

    pixel_buffer[y * VGA_WIDTH + x] = colour;
}

void VGA_clear_screen(uint16_t colour)
{
    int x, y;
    for (y = 0; y < VGA_HEIGHT; y++)
    {
        for (x = 0; x < VGA_WIDTH; x++)
        {
            pixel_buffer[y * VGA_WIDTH + x] = colour;
        }
    }
}

void VGA_draw_hline(int x1, int x2, int y, uint16_t colour)
{
    int x, temp;

    if (y < 0 || y >= VGA_HEIGHT)
        return;

    if (x1 > x2)
    {
        temp = x1;
        x1 = x2;
        x2 = temp;
    }

    if (x1 < 0) x1 = 0;
    if (x2 >= VGA_WIDTH) x2 = VGA_WIDTH - 1;

    for (x = x1; x <= x2; x++)
    {
        VGA_plot_pixel(x, y, colour);
    }
}

void VGA_draw_vline(int x, int y1, int y2, uint16_t colour)
{
    int y, temp;

    if (x < 0 || x >= VGA_WIDTH)
        return;

    if (y1 > y2)
    {
        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    if (y1 < 0) y1 = 0;
    if (y2 >= VGA_HEIGHT) y2 = VGA_HEIGHT - 1;

    for (y = y1; y <= y2; y++)
    {
        VGA_plot_pixel(x, y, colour);
    }
}

void VGA_draw_rect(int x, int y, int width, int height, uint16_t colour)
{
    if (width <= 0 || height <= 0)
        return;

    VGA_draw_hline(x, x + width - 1, y, colour);
    VGA_draw_hline(x, x + width - 1, y + height - 1, colour);
    VGA_draw_vline(x, y, y + height - 1, colour);
    VGA_draw_vline(x + width - 1, y, y + height - 1, colour);
}

void VGA_fill_rect(int x, int y, int width, int height, uint16_t colour)
{
    int row;

    if (width <= 0 || height <= 0)
        return;

    for (row = 0; row < height; row++)
    {
        VGA_draw_hline(x, x + width - 1, y + row, colour);
    }
}

/* Bresenham line algorithm */
void VGA_draw_line(int x0, int y0, int x1, int y1, uint16_t colour)
{
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = (y1 > y0) ? -(y1 - y0) : -(y0 - y1);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1)
    {
        VGA_plot_pixel(x0, y0, colour);

        if (x0 == x1 && y0 == y1)
            break;

        e2 = 2 * err;

        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}
