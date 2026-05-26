/*
 * dashboard.c
 *
 * VGA dashboard implementation. See dashboard.h for the layout map.
 *
 * Assumes the host file already defines:
 *   - write_pixel(unsigned int addr, unsigned char color)
 *   - sdram_set_pixel(int x, int y, unsigned char color)
 *   - clear_sdram_image(unsigned char color)
 *   - display_sdram_image(void)
 *   - draw_string_to_sdram(int x, int y, const char *text,
 *                          unsigned char color, int scale)
 *   - VGA_WIDTH, VGA_HEIGHT macros (320, 240)
 *   - font5x7[][] table for ASCII subset (space, A-Z, 0-9)
 *
 * This file only adds the dashboard layer on top.
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "dashboard.h"

/* These are defined in the host main file. */
extern void write_pixel(unsigned int addr, unsigned char color);
extern void sdram_set_pixel(int x, int y, unsigned char color);
extern void clear_sdram_image(unsigned char color);
extern void display_sdram_image(void);
extern void draw_string_to_sdram(int x, int y, const char *text,
                                 unsigned char color, int scale);
extern int  get_font_index(char c);
extern const uint8_t font5x7[37][7];

/* ---------------- Geometry ---------------- */

#define VGA_W           320
#define VGA_H           240

/* Title bar */
#define TITLE_Y0        0
#define TITLE_Y1        15

/* VGA monitor */
#define MON_X0          2
#define MON_Y0          18
#define MON_X1          158
#define MON_Y1          118
#define MON_INNER_X0    (MON_X0 + 4)
#define MON_INNER_Y0    (MON_Y0 + 12)
#define MON_INNER_X1    (MON_X1 - 4)
#define MON_INNER_Y1    (MON_Y1 - 4)

/* Accelerometer */
#define ACC_X0          160
#define ACC_Y0          18
#define ACC_X1          318
#define ACC_Y1          58

/* System status */
#define STAT_X0         160
#define STAT_Y0         62
#define STAT_X1         318
#define STAT_Y1         118

/* LEDs */
#define LED_X0          2
#define LED_Y0          122
#define LED_X1          158
#define LED_Y1          158

/* Switches */
#define SW_X0           160
#define SW_Y0           122
#define SW_X1           318
#define SW_Y1           158

/* Keys */
#define KEY_X0          2
#define KEY_Y0          162
#define KEY_X1          158
#define KEY_Y1          198

/* Hex */
#define HEX_X0          160
#define HEX_Y0          162
#define HEX_X1          318
#define HEX_Y1          198

/* ---------------- Color palette (4-bit) ---------------- */
/*
 * The exact palette depends on the VGA controller LUT. These names
 * are best-effort guesses based on typical 4-bit IRGB / RRGB layouts.
 * [Unverified] If colors look wrong, remap the constants here.
 */
#define COL_BLACK       0x0
#define COL_DK_BLUE     0x1
#define COL_DK_GREEN    0x2
#define COL_DK_CYAN     0x3
#define COL_DK_RED      0x4
#define COL_DK_MAGENTA  0x5
#define COL_DK_YELLOW   0x6
#define COL_GRAY        0x7
#define COL_DK_GRAY     0x8
#define COL_BLUE        0x9
#define COL_GREEN       0xA
#define COL_CYAN        0xB
#define COL_RED         0xC
#define COL_MAGENTA     0xD
#define COL_YELLOW      0xE
#define COL_WHITE       0xF

/* ---------------- Drawing primitives (write directly to VGA ctrl) ---------------- */

static inline void px(int x, int y, unsigned char color)
{
    if (x < 0 || x >= VGA_W || y < 0 || y >= VGA_H) return;
    write_pixel((unsigned int)(y * VGA_W + x), color);
}

static void fill_rect(int x0, int y0, int x1, int y1, unsigned char color)
{
    if (x0 > x1) { int t = x0; x0 = x1; x1 = t; }
    if (y0 > y1) { int t = y0; y0 = y1; y1 = t; }
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            px(x, y, color);
        }
    }
}

static void rect_outline(int x0, int y0, int x1, int y1, unsigned char color)
{
    for (int x = x0; x <= x1; x++) {
        px(x, y0, color);
        px(x, y1, color);
    }
    for (int y = y0; y <= y1; y++) {
        px(x0, y, color);
        px(x1, y, color);
    }
}

static void draw_char_to_vga(int x, int y, char c,
                             unsigned char color, int scale)
{
    int idx = get_font_index(c);
    for (int row = 0; row < 7; row++) {
        uint8_t row_data = font5x7[idx][row];
        for (int col = 0; col < 5; col++) {
            int bit = (row_data >> (4 - col)) & 0x01;
            if (bit) {
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        px(x + col * scale + sx,
                           y + row * scale + sy,
                           color);
                    }
                }
            }
        }
    }
}

static void draw_string_to_vga(int x, int y, const char *text,
                               unsigned char color, int scale)
{
    int cx = x;
    int char_w = 5 * scale;
    int spacing = scale;
    while (*text) {
        if (*text != '\n') {
            draw_char_to_vga(cx, y, *text, color, scale);
            cx += char_w + spacing;
        }
        text++;
    }
}

/* Erase a text slot then redraw. width_chars must cover the longest
 * string ever drawn into this slot. */
static void redraw_text_slot(int x, int y, int width_chars, int scale,
                             const char *text, unsigned char color,
                             unsigned char bg)
{
    int w_px = width_chars * (5 * scale + scale);
    int h_px = 7 * scale;
    fill_rect(x, y, x + w_px - 1, y + h_px - 1, bg);
    draw_string_to_vga(x, y, text, color, scale);
}

/* Filled disc (used for LED dots). Cheap radius<=8 implementation. */
static void fill_disc(int cx, int cy, int r, unsigned char color)
{
    int r2 = r * r;
    for (int dy = -r; dy <= r; dy++) {
        for (int dx = -r; dx <= r; dx++) {
            if (dx * dx + dy * dy <= r2) {
                px(cx + dx, cy + dy, color);
            }
        }
    }
}

static void circle_outline(int cx, int cy, int r, unsigned char color)
{
    /* Midpoint circle algorithm. */
    int x = r, y = 0, err = 0;
    while (x >= y) {
        px(cx + x, cy + y, color);
        px(cx + y, cy + x, color);
        px(cx - y, cy + x, color);
        px(cx - x, cy + y, color);
        px(cx - x, cy - y, color);
        px(cx - y, cy - x, color);
        px(cx + y, cy - x, color);
        px(cx + x, cy - y, color);
        y++;
        if (err <= 0) { err += 2 * y + 1; }
        if (err > 0)  { x--; err -= 2 * x + 1; }
    }
}

/* ---------------- Static frame: composed in SDRAM, displayed once ---------------- */

/* These helpers write to SDRAM (the scratch buffer). They are only used
 * during dashboard_init_static(). */

static void sdram_fill_rect(int x0, int y0, int x1, int y1, unsigned char color)
{
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            sdram_set_pixel(x, y, color);
        }
    }
}

static void sdram_rect_outline(int x0, int y0, int x1, int y1, unsigned char color)
{
    for (int x = x0; x <= x1; x++) {
        sdram_set_pixel(x, y0, color);
        sdram_set_pixel(x, y1, color);
    }
    for (int y = y0; y <= y1; y++) {
        sdram_set_pixel(x0, y, color);
        sdram_set_pixel(x1, y, color);
    }
}

void dashboard_init_static(void)
{
    /* Wipe scratch buffer to black. */
    clear_sdram_image(COL_BLACK);

    /* ---- Title bar ---- */
    sdram_fill_rect(0, TITLE_Y0, VGA_W - 1, TITLE_Y1, COL_DK_BLUE);
    /* Centered: "CPU DASHBOARD NIOS II GROUP 1" (29 chars at scale 1)
     * width = 29 * (5 + 1) = 174 px; centered at (320 - 174) / 2 = 73. */
    draw_string_to_sdram(73, 4, "CPU DASHBOARD NIOS II GROUP 1",
                         COL_YELLOW, 1);

    /* ---- VGA monitor panel ---- */
    sdram_rect_outline(MON_X0, MON_Y0, MON_X1, MON_Y1, COL_BLUE);
    /* Header label */
    draw_string_to_sdram(MON_X0 + 40, MON_Y0 + 2, "VGA MONITOR",
                         COL_CYAN, 1);
    /* Inner canvas border (camera preview area) */
    sdram_rect_outline(MON_INNER_X0 - 1, MON_INNER_Y0 - 1,
                       MON_INNER_X1 + 1, MON_INNER_Y1 + 1, COL_DK_GRAY);
    /* Inner canvas filled black (this is what the scan line sweeps over) */
    sdram_fill_rect(MON_INNER_X0, MON_INNER_Y0,
                    MON_INNER_X1, MON_INNER_Y1, COL_BLACK);

    /* ---- Accelerometer panel ---- */
    sdram_rect_outline(ACC_X0, ACC_Y0, ACC_X1, ACC_Y1, COL_MAGENTA);
    draw_string_to_sdram(ACC_X0 + 36, ACC_Y0 + 2, "ACCELEROMETER",
                         COL_MAGENTA, 1);
    /* Static labels: P: R: Z: TILT: */
    draw_string_to_sdram(ACC_X0 + 4,  ACC_Y0 + 14, "P", COL_GREEN, 1);
    draw_string_to_sdram(ACC_X0 + 4,  ACC_Y0 + 14, "P", COL_GREEN, 1);
    /* (TILT label drawn at static position too) */
    draw_string_to_sdram(ACC_X0 + 4,  ACC_Y0 + 26, "TILT", COL_GRAY, 1);

    /* ---- System status panel ---- */
    sdram_rect_outline(STAT_X0, STAT_Y0, STAT_X1, STAT_Y1, COL_GREEN);
    draw_string_to_sdram(STAT_X0 + 36, STAT_Y0 + 2, "SYSTEM STATUS",
                         COL_GREEN, 1);
    /* Static labels */
    draw_string_to_sdram(STAT_X0 + 4,  STAT_Y0 + 14, "SPI",  COL_GREEN, 1);
    draw_string_to_sdram(STAT_X0 + 76, STAT_Y0 + 14, "EMG",  COL_GREEN, 1);
    draw_string_to_sdram(STAT_X0 + 4,  STAT_Y0 + 26, "RD",   COL_GREEN, 1);
    draw_string_to_sdram(STAT_X0 + 4,  STAT_Y0 + 38, "EMG",  COL_GREEN, 1);

    /* ---- LEDs panel ---- */
    sdram_rect_outline(LED_X0, LED_Y0, LED_X1, LED_Y1, COL_DK_YELLOW);
    draw_string_to_sdram(LED_X0 + 52, LED_Y0 + 2, "LEDS 7 0",
                         COL_YELLOW, 1);

    /* ---- Switches panel ---- */
    sdram_rect_outline(SW_X0, SW_Y0, SW_X1, SW_Y1, COL_DK_MAGENTA);
    draw_string_to_sdram(SW_X0 + 36, SW_Y0 + 2, "SWITCHES 7 0",
                         COL_MAGENTA, 1);

    /* ---- Keys panel ---- */
    sdram_rect_outline(KEY_X0, KEY_Y0, KEY_X1, KEY_Y1, COL_BLUE);
    draw_string_to_sdram(KEY_X0 + 40, KEY_Y0 + 2, "PUSHBUTTONS",
                         COL_CYAN, 1);

    /* ---- Hex panel ---- */
    sdram_rect_outline(HEX_X0, HEX_Y0, HEX_X1, HEX_Y1, COL_MAGENTA);
    draw_string_to_sdram(HEX_X0 + 52, HEX_Y0 + 2, "HEX 5 0",
                         COL_MAGENTA, 1);

    /* Push the whole composed frame to the VGA controller. */
    display_sdram_image();
}

/* ---------------- Dynamic updates: write_pixel() directly to VGA ---------------- */

/* ----- LEDs -----
 * 8 LEDs across 156 px panel.
 * LED 7 is leftmost. Spacing chosen so each LED has a ~16 px column.
 * LED radius = 5 logical px. Row y = 140 (center).
 */
static const int LED_CENTERS_X[8] = {
    /* LED 7  6   5   4    3    2    1    0  */
       18, 36, 54, 72, 90, 108, 126, 144
};
#define LED_CY      141
#define LED_R       5

void dashboard_update_leds(uint8_t led_state)
{
    /* led_state bit 0 = LED0 (rightmost), bit 7 = LED7 (leftmost). */
    for (int i = 0; i < 8; i++) {
        int bit_index = 7 - i;  /* LED 7 drawn first (leftmost). */
        int on = (led_state >> bit_index) & 0x1;
        unsigned char fill = on ? COL_RED : COL_DK_GRAY;
        int cx = LED_X0 + LED_CENTERS_X[i];
        fill_disc(cx, LED_CY, LED_R, fill);
        circle_outline(cx, LED_CY, LED_R, COL_WHITE);
    }
}

/* ----- Switches -----
 * 8 toggle graphics, 7-bit wide each, spaced ~16 px apart.
 * Each switch is an 8x15 outlined slot; the slider fills the upper
 * half when ON, lower half when OFF.
 */
static const int SW_CENTERS_X[8] = {
       18, 36, 54, 72, 90, 108, 126, 144
};
#define SW_TOP      133
#define SW_BOT      152
#define SW_HALF_W   3

void dashboard_update_switches(uint8_t sw_state)
{
    for (int i = 0; i < 8; i++) {
        int bit_index = 7 - i;
        int on = (sw_state >> bit_index) & 0x1;
        int cx = SW_X0 + SW_CENTERS_X[i];

        /* Slot background and frame */
        fill_rect(cx - SW_HALF_W, SW_TOP,
                  cx + SW_HALF_W, SW_BOT, COL_DK_GRAY);
        rect_outline(cx - SW_HALF_W, SW_TOP,
                     cx + SW_HALF_W, SW_BOT, COL_WHITE);

        /* Slider: upper half if ON, lower half if OFF */
        int mid = (SW_TOP + SW_BOT) / 2;
        if (on) {
            fill_rect(cx - SW_HALF_W + 1, SW_TOP + 1,
                      cx + SW_HALF_W - 1, mid, COL_GREEN);
        } else {
            fill_rect(cx - SW_HALF_W + 1, mid,
                      cx + SW_HALF_W - 1, SW_BOT - 1, COL_GRAY);
        }
    }
}

/* ----- Keys ----- */
#define KEY0_CX     46
#define KEY1_CX     112
#define KEY_CY      182
#define KEY_R       12

void dashboard_update_keys(int key0_pressed, int key1_pressed)
{
    /* KEY0 */
    fill_disc(KEY0_CX, KEY_CY, KEY_R,
              key0_pressed ? COL_YELLOW : COL_DK_BLUE);
    circle_outline(KEY0_CX, KEY_CY, KEY_R, COL_CYAN);
    /* Label centered over the disc: "KEY0" = 4 chars, scale 1
     * width = 4*6 = 24 px, so start at cx - 12. */
    draw_string_to_vga(KEY0_CX - 12, KEY_CY - 3, "KEY0",
                       key0_pressed ? COL_BLACK : COL_YELLOW, 1);

    /* KEY1 */
    fill_disc(KEY1_CX, KEY_CY, KEY_R,
              key1_pressed ? COL_RED : COL_DK_RED);
    circle_outline(KEY1_CX, KEY_CY, KEY_R, COL_RED);
    draw_string_to_vga(KEY1_CX - 12, KEY_CY - 3, "KEY1",
                       COL_WHITE, 1);
}

/* ----- Accelerometer ----- */

static void format_signed_3(int v, char *out)
{
    /* "+023" / "-007" style, 4 chars + NUL. Clamped to 3 digits. */
    int neg = v < 0;
    int mag = neg ? -v : v;
    if (mag > 999) mag = 999;
    out[0] = neg ? '-' : '+';
    out[1] = '0' + (mag / 100);
    out[2] = '0' + ((mag / 10) % 10);
    out[3] = '0' + (mag % 10);
    out[4] = '\0';
}

void dashboard_update_accel(int pitch, int roll, int z, int tilt_safe)
{
    char buf[8];

    /* Three numeric slots: P R Z. Each is 4 chars at scale 1. */
    format_signed_3(pitch, buf);
    redraw_text_slot(ACC_X0 + 14, ACC_Y0 + 14, 4, 1,
                     buf, COL_YELLOW, COL_BLACK);

    /* Roll: re-label "R" then number. Static "P" label drawn at init
     * was wrong for roll slot. Draw "R" + "Z" labels too. */
    draw_string_to_vga(ACC_X0 + 50, ACC_Y0 + 14, "R", COL_GREEN, 1);
    format_signed_3(roll, buf);
    redraw_text_slot(ACC_X0 + 60, ACC_Y0 + 14, 4, 1,
                     buf, COL_YELLOW, COL_BLACK);

    draw_string_to_vga(ACC_X0 + 96, ACC_Y0 + 14, "Z", COL_GREEN, 1);
    format_signed_3(z, buf);
    redraw_text_slot(ACC_X0 + 106, ACC_Y0 + 14, 4, 1,
                     buf, COL_YELLOW, COL_BLACK);

    /* Tilt status */
    if (tilt_safe) {
        redraw_text_slot(ACC_X0 + 40, ACC_Y0 + 26, 6, 1,
                         "SAFE  ", COL_GREEN, COL_BLACK);
    } else {
        redraw_text_slot(ACC_X0 + 40, ACC_Y0 + 26, 6, 1,
                         "DANGER", COL_RED, COL_BLACK);
    }
}

/* ----- System status ----- */

void dashboard_update_status(int spi_running,
                             int emergency,
                             const char *reading_5digit)
{
    /* SPI: RUN or STOP */
    redraw_text_slot(STAT_X0 + 28, STAT_Y0 + 14, 4, 1,
                     spi_running ? "RUN " : "STOP",
                     spi_running ? COL_GREEN : COL_RED,
                     COL_BLACK);

    /* EMG: CLR or HALT */
    redraw_text_slot(STAT_X0 + 100, STAT_Y0 + 14, 4, 1,
                     emergency ? "HALT" : "CLR ",
                     emergency ? COL_RED : COL_GREEN,
                     COL_BLACK);

    /* RD: 5-digit reading (or blanks if NULL/empty) */
    char rd_buf[8];
    if (reading_5digit && reading_5digit[0]) {
        strncpy(rd_buf, reading_5digit, 5);
        rd_buf[5] = '\0';
    } else {
        strcpy(rd_buf, "-----");
    }
    redraw_text_slot(STAT_X0 + 28, STAT_Y0 + 26, 5, 1,
                     rd_buf, COL_YELLOW, COL_BLACK);
}

/* ----- HEX displays ----- */
/*
 * 6 digit cells in the HEX panel.
 * Panel inner: x 162..316 (155 px), y 174..196 (23 px).
 * Each cell ~25 px wide. Digit drawn with scale 2 (10x14 px glyph),
 * leaves ~7 px horizontal padding.
 */
static const int HEX_CELL_X[6] = {
    /* HEX5  4    3    2    1    0 */
       2,  27,  52,  77, 102, 127
};
#define HEX_CELL_W   23
#define HEX_CELL_H   22
#define HEX_DIGIT_SCALE 2

void dashboard_update_hex(const uint8_t hex_digits[6])
{
    for (int i = 0; i < 6; i++) {
        int cx0 = HEX_X0 + HEX_CELL_X[i];
        int cy0 = HEX_Y0 + 12;

        /* Cell background */
        fill_rect(cx0, cy0, cx0 + HEX_CELL_W, cy0 + HEX_CELL_H,
                  COL_BLACK);
        rect_outline(cx0, cy0, cx0 + HEX_CELL_W, cy0 + HEX_CELL_H,
                     COL_DK_GRAY);

        /* Digit. Treat values > 9 as 'blank' (you can extend the
         * font to A..F later if needed). */
        uint8_t v = hex_digits[i];
        if (v <= 9) {
            char glyph = (char)('0' + v);
            char s[2] = { glyph, '\0' };
            /* Center the 10x14 glyph in the 23x22 cell. */
            int tx = cx0 + (HEX_CELL_W - 10) / 2;
            int ty = cy0 + (HEX_CELL_H - 14) / 2;
            draw_string_to_vga(tx, ty, s, COL_RED, HEX_DIGIT_SCALE);
        }
    }
}

/* ---------------- Scan line ----------------
 * Sweeps a 1-px green line top-to-bottom inside the VGA monitor
 * inner canvas. Each call:
 *   1. Erases the previous line (paints it black).
 *   2. Advances to next row.
 *   3. Draws the new line.
 * Wraps when it reaches the bottom.
 */
static int scanline_y = MON_INNER_Y0;

void dashboard_tick_scanline(void)
{
    /* Erase old line */
    for (int x = MON_INNER_X0; x <= MON_INNER_X1; x++) {
        px(x, scanline_y, COL_BLACK);
    }

    /* Advance */
    scanline_y++;
    if (scanline_y > MON_INNER_Y1) {
        scanline_y = MON_INNER_Y0;
    }

    /* Draw new line */
    for (int x = MON_INNER_X0; x <= MON_INNER_X1; x++) {
        px(x, scanline_y, COL_GREEN);
    }
}
