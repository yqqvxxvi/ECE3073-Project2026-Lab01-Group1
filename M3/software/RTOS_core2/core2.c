#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "system.h"
#include <io.h>
#include <sys/alt_irq.h>
#include <sys/alt_alarm.h>
#include <sys/alt_cache.h>
#include <sys/alt_stdio.h>

#include <altera_avalon_jtag_uart_regs.h>
#include "altera_avalon_mutex.h"

#include "altera_up_avalon_accelerometer_spi.h"
#include "sdram.h"
#include "hex.h"
#include "accelerometer.h"
#include "nios2_ctrl_reg_macros.h"

#ifndef OS_TICKS_PER_SEC
#define OS_TICKS_PER_SEC 100  /* Fixes the compiler check manually */
#endif

#define SDRAM_1       				0x01100000
#define SDRAM_2       				0x01200000


//volatile int *Keys       = (int *)KEY01_BASE;

volatile int *Mutex      = (int *)MUTEX_BASE;
volatile int *Shared_Sdram_Flag      = (int *)SHARED_SDRAM_BASE;
volatile int *Shared_Sdram_1      = (int *)SDRAM_1;
volatile int *Shared_Sdram_2      = (int *)SDRAM_2;
//
#define HEX345_BASE 0x21020
#define HEX012_BASE 0x21030
#define FRAMEBUFFER_BASE  0x01300000   // pick a free region

//
//HEX345_BASE

volatile int *timer      = (int *)US_COUNTER_BASE;
volatile int *OutPort_HEX012      = (int *)HEX012_BASE;
volatile int *OutPort_HEX345      = (int *)HEX345_BASE;
volatile int *ACCEL      = (int *)ACCELEROMETER_SPI_0_BASE;
volatile int *SPI_RxData  = (int *) SPI_0_BASE;
volatile int *SPI_TxData  = (int *) (SPI_0_BASE + 0x4);
volatile int *SPI_Status  = (int *) (SPI_0_BASE + 0x8);
volatile int *SPI_Control = (int *) (SPI_0_BASE + 0xC);
volatile int *SPI_SS      = (int *)SPI_SS_BASE;

#define SPI_STATUS_ADDR SPI_Status
#define SPI_RXDATA_ADDR SPI_RxData
#define SPI_TXDATA_ADDR SPI_TxData
#define SPI_SS_ADDR SPI_SS_BASE
#define SPI_CONTROL_ADDR SPI_Control
#define SPI_SS_ADDR SPI_SS_BASE

void accelerometer_main(float *pitch, float *roll, float *z);

/////////////////////////////////////////////////////////////////////////
#define VGA_WIDTH        320
#define VGA_HEIGHT       240
#define VGA_PIXEL_COUNT  (VGA_WIDTH * VGA_HEIGHT)

//static volatile uint8_t * const SDRAM_FRAME =
//    (volatile uint8_t *)(IMAGE_ADDRESS_BASE);

static volatile uint8_t * const SDRAM_FRAME =
    (volatile uint8_t *)(FRAMEBUFFER_BASE);
/*
   5x7 font lookup table.
   Each byte is one row.
   Only lower 5 bits are used.

   Example:
   0x0E = 01110
   0x11 = 10001
*/
const uint8_t font5x7[37][7] =
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
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F},
    /* 0 */
    {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E},
    /* 1 */
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},
    /* 2 */
    {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F},
    /* 3 */
    {0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E},
    /* 4 */
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02},
    /* 5 */
    {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E},
    /* 6 */
    {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E},
    /* 7 */
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},
    /* 8 */
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E},
    /* 9 */
    {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C}
};

void write_pixel(unsigned int addr, unsigned char color)
{
    IOWR_32DIRECT(IMAGE_ADDRESS_BASE, 0, (addr));
    IOWR_32DIRECT(PIXEL_BUFFER_BASE,  0, color & 0xF);

    IOWR_32DIRECT(WERN_BASE, 0, 1);

    IOWR_32DIRECT(WERN_BASE, 0, 0);
}

void draw_horizontal_stripes(void)
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

    if (c >= '0' && c <= '9')
        return c - '0' + 27;

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

void generate_text_image_to_sdram(char *reading)
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
    draw_string_to_sdram(30, 95, "SPI INFERRED RESULT", 0xE, 2);
    draw_string_to_sdram(30, 125, reading, 0xC, 2);

}

/* =========================================================
 *  SCREEN 1: WELCOME SCREEN
 *  - Dark background
 *  - Bright border
 *  - Title text + subtitle
 * ========================================================= */
void draw_welcome_screen(void)
{
    clear_sdram_image(0x1);   /* dark blue-ish base */

    /* Soft diagonal gradient background */
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            unsigned char bg = ((x + y) / 30) & 0x7;   /* darker tones only */
            sdram_set_pixel(x, y, bg);
        }
    }

    /* Double border */
    for (int x = 0; x < VGA_WIDTH; x++) {
        sdram_set_pixel(x, 0, 0xF);
        sdram_set_pixel(x, 1, 0xF);
        sdram_set_pixel(x, VGA_HEIGHT - 1, 0xF);
        sdram_set_pixel(x, VGA_HEIGHT - 2, 0xF);
    }
    for (int y = 0; y < VGA_HEIGHT; y++) {
        sdram_set_pixel(0, y, 0xF);
        sdram_set_pixel(1, y, 0xF);
        sdram_set_pixel(VGA_WIDTH - 1, y, 0xF);
        sdram_set_pixel(VGA_WIDTH - 2, y, 0xF);
    }

    /* Inner accent line */
    for (int x = 10; x < VGA_WIDTH - 10; x++) {
        sdram_set_pixel(x, 40,  0xE);
        sdram_set_pixel(x, VGA_HEIGHT - 40, 0xE);
    }

    /* Title:  "WELCOME"  big, centered-ish */
    /* Each char at scale=3 is 15 wide + 3 spacing = 18 px, 7 chars ~ 126 px */
    draw_string_to_sdram(95,  70, "WELCOME", 0xF, 3);

    /* Subtitle: "WATER METER SYSTEM" smaller */
    /* scale=2 char width = 10 + 2 = 12 px, 18 chars ~ 216 px */
    draw_string_to_sdram(50, 130, "WATER METER SYSTEM", 0xE, 2);

    /* Footer */
    draw_string_to_sdram(80, 180, "PRESS KEY TO BEGIN", 0xC, 1);

    display_sdram_image();
}

/* =========================================================
 *  SCREEN 2: OPERATING SCREEN
 *  - Calm green/teal palette
 *  - "SYSTEM OPERATING" header
 *  - Status lines
 * ========================================================= */
void draw_operating_screen(void)
{
    clear_sdram_image(0x0);

    /* Background: horizontal bands in cool tones */
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        unsigned char bg = (y / 20) & 0x3;   /* keep dark, 0..3 */
        for (int x = 0; x < VGA_WIDTH; x++) {
            sdram_set_pixel(x, y, bg);
        }
    }

    /* Top header bar */
    for (int y = 0; y < 30; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            sdram_set_pixel(x, y, 0xA);   /* green-ish */
        }
    }

    /* Bottom status bar */
    for (int y = VGA_HEIGHT - 20; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            sdram_set_pixel(x, y, 0xA);
        }
    }

    /* Border */
    for (int x = 0; x < VGA_WIDTH; x++) {
        sdram_set_pixel(x, 0, 0xF);
        sdram_set_pixel(x, VGA_HEIGHT - 1, 0xF);
    }
    for (int y = 0; y < VGA_HEIGHT; y++) {
        sdram_set_pixel(0, y, 0xF);
        sdram_set_pixel(VGA_WIDTH - 1, y, 0xF);
    }

    /* Header text:  "SYSTEM OPERATING"  scale=2 */
    draw_string_to_sdram(60, 8, "SYSTEM OPERATING", 0xF, 2);

    /* Body status lines (scale=1) */
    draw_string_to_sdram(20,  60, "STATUS      RUNNING",   0xF, 1);
    draw_string_to_sdram(20,  85, "SCANNING    ACTIVE",    0xE, 1);
    draw_string_to_sdram(20, 110, "SENSOR      ONLINE",    0xE, 1);
    draw_string_to_sdram(20, 135, "EMERGENCY   CLEAR",     0xA, 1);

    /* Footer text */
    draw_string_to_sdram(70, VGA_HEIGHT - 14, "MONITORING IN PROGRESS", 0x0, 1);

    display_sdram_image();
}

/* =========================================================
 *  SCREEN 3: EMERGENCY SCREEN
 *  - Aggressive red palette
 *  - Big "EMERGENCY STOP" text
 *  - Warning chevrons on the sides
 * ========================================================= */
void draw_emergency_screen(void)
{
    clear_sdram_image(0x4);   /* red-ish base depends on your palette */

    /* Fill background red */
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            sdram_set_pixel(x, y, 0x4);
        }
    }

    /* Diagonal hazard stripes top + bottom */
    for (int y = 0; y < 25; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            unsigned char c = (((x + y) / 10) & 0x1) ? 0xF : 0x0;
            sdram_set_pixel(x, y, c);
            sdram_set_pixel(x, VGA_HEIGHT - 1 - y, c);
        }
    }

    /* Thick border */
    for (int t = 0; t < 4; t++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            sdram_set_pixel(x, 25 + t, 0xF);
            sdram_set_pixel(x, VGA_HEIGHT - 26 - t, 0xF);
        }
        for (int y = 25; y < VGA_HEIGHT - 25; y++) {
            sdram_set_pixel(t, y, 0xF);
            sdram_set_pixel(VGA_WIDTH - 1 - t, y, 0xF);
        }
    }

    /* Big "EMERGENCY" title scale=3 */
    /* 9 chars * (15+3) = 162 px wide, center ~ (320-162)/2 = 79 */
    draw_string_to_sdram(79, 70, "EMERGENCY", 0xF, 3);

    /* "STOP" even bigger, scale=4 */
    /* 4 chars * (20+4) = 96 px wide, center ~ (320-96)/2 = 112 */
    draw_string_to_sdram(112, 120, "STOP", 0xF, 4);

    /* Warning footer */
    draw_string_to_sdram(50, 175, "TILT DETECTED HALT SYSTEM", 0xF, 1);

    display_sdram_image();
}

/* ---------------- Safe Nios II/f I/O access macros ---------------- */
#define READ_REG(addr)         IORD(addr, 0)
#define WRITE_REG(addr, val)   IOWR(addr, 0, val)

/* ---------------- SPI bits ---------------- */
#define TRDY 0x40
#define RRDY 0x80

#define SPI_CONTROL_RRDY_INT_ENABLE 0x80

/* ---------------- SDRAM layout ---------------- */
#define SDRAM_READING_OFFSET  0
#define STORED_READING_LEN    6     /* 5 digits + '\0' */

/* ---------------- Timing ---------------- */
#define SPI_SS_DELAY          1000
#define ESP32_QUEUE_DELAY     8000
#define MAIN_LOOP_DELAY       5000
#define KEY_DEBOUNCE_DELAY    30000

/* 3-second user action window */
#define ACTION_WINDOW_SECONDS 3

/*
 * Fallback only used if alt_ticks_per_second() is unavailable/zero.
 * This is approximate.
 */
#define ACTION_WINDOW_FALLBACK_LOOPS 30000

/* ---------------- Water meter frame ---------------- */
#define FRAME_END             '\n'
#define CARRIAGE_RETURN       '\r'
#define NO_READING_BYTE       'N'
#define IDLE_SPI_BYTE         0xFF
#define MAX_READING_LEN       16
#define REQUIRED_DIGITS       5

#define DEBUG_SPI_BYTES       0

/* ---------------- Speaker / Morse tuning ---------------- */
#define SPEAKER_ON_VALUE      1
#define SPEAKER_OFF_VALUE     0

#define LED_ON_VALUE          1
#define LED_OFF_VALUE         0

#define MORSE_UNIT_DELAY      150000
#define MORSE_DOT_TOGGLES     300
#define TONE_HALF_PERIOD      500

/* ---------------- Global SPI state ---------------- */
volatile uint8_t latest_spi_byte = 0;

volatile int spi_running = 0;
volatile int spi_byte_ready = 0;
volatile int spi_next_byte_needed = 0;

/*
 * When this is 1:
 * - A valid 5-digit reading has been captured.
 * - SPI scanning is paused.
 * - User has 3 seconds to press KEY0 or KEY1.
 */
volatile int reading_locked_waiting_for_action = 0;

/*
 * New state:
 * If KEY0 is pressed and Morse has played,
 * the system waits indefinitely for KEY1.
 *
 * While this is 1:
 * - The 3-second timeout is disabled.
 * - Scanning remains paused.
 * - KEY1 stores the reading to SDRAM and resumes scanning.
 */
volatile int morse_done_waiting_for_key1 = 0;

/* Action window timing */
alt_u32 action_window_start_ticks = 0;
int action_window_uses_ticks = 0;
int action_window_fallback_counter = 0;

/* ---------------- Reading buffers ---------------- */
char reading_buffer[MAX_READING_LEN];
int reading_index = 0;

char last_printed_reading[MAX_READING_LEN] = "";

/*
 * Latest valid 5-digit water meter reading.
 * Example: "02012"
 */
char current_5digit_reading[STORED_READING_LEN] = "";
int valid_5digit_available = 0;

/* ---------------- Helper delays ---------------- */
void short_delay(volatile int count)
{
    for (volatile int i = 0; i < count; i++);
}


/* ---------------- Reset partial frame ---------------- */
void reset_reading_buffer(void)
{
    reading_index = 0;
    reading_buffer[0] = '\0';
}

/* ---------------- SDRAM functions ---------------- */
void store_current_reading_to_sdram(void)
{
    if (!valid_5digit_available) {
        printf("No valid 5-digit reading available to store.\n");
        return;
    }

    sdram_write_array(
        SDRAM_READING_OFFSET,
        (uint8_t *)current_5digit_reading,
        strlen(current_5digit_reading) + 1
    );

    printf("Stored 5-digit reading into SDRAM: %s\n", current_5digit_reading);
    printf("-------------------------\n");

//    WRITE_REG(GREEN_LED_ADDR, LED_ON_VALUE);
//    WRITE_REG(OUTPORT_LED_ADDR, 0x0F);
//
//    beep_short();
//
//    WRITE_REG(GREEN_LED_ADDR, LED_OFF_VALUE);
//    WRITE_REG(OUTPORT_LED_ADDR, 0x00);
}

void print_reading_from_sdram(void)
{
    char stored_reading[STORED_READING_LEN];

    memset(stored_reading, 0, sizeof(stored_reading));

    sdram_read_string(
        SDRAM_READING_OFFSET,
        stored_reading,
        sizeof(stored_reading)
    );

    printf("Reading currently stored in SDRAM: %s\n", stored_reading);
    printf("-------------------------\n");
}

/* ---------------- Validation helpers ---------------- */
int is_all_digits(const char *s)
{
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '9') {
            return 0;
        }
    }

    return 1;
}

int is_valid_5digit_reading(const char *reading)
{
    if (strlen(reading) <= REQUIRED_DIGITS) {
        return 0;
    }

    return is_all_digits(reading);
}

/* ---------------- SPI helpers ---------------- */
void spi_flush_rx(void)
{
    while (READ_REG(SPI_STATUS_ADDR) & RRDY) {
        (void)(uint8_t)(READ_REG(SPI_RXDATA_ADDR) & 0xFF);
    }
}

void spi_start_one_byte_transfer(uint8_t tx)
{
    WRITE_REG(SPI_SS, 0x0);
    short_delay(SPI_SS_DELAY);

    while (!(READ_REG(SPI_STATUS_ADDR) & TRDY));

    WRITE_REG(SPI_TXDATA_ADDR, tx);
}

/* ---------------- Lock / unlock scanning ---------------- */
void stop_spi_reading_for_action_window(void)
{
    spi_running = 0;
    spi_next_byte_needed = 0;
    spi_byte_ready = 0;

    WRITE_REG(SPI_SS_ADDR, 0x1);

    spi_flush_rx();

    action_window_start_ticks = alt_nticks();

    if (alt_ticks_per_second() > 0) {
        action_window_uses_ticks = 1;
    }
    else {
        action_window_uses_ticks = 0;
    }

    action_window_fallback_counter = 0;

    morse_done_waiting_for_key1 = 0;

    printf("SPI scanning paused for 3 seconds.\n");
    printf("Press KEY0 for Morse or KEY1 to store to SDRAM.\n");
    printf("If no key is pressed, scanning will resume automatically.\n");
    printf("-------------------------\n");

}

void resume_spi_reading_after_action_window(void)
{
    printf("Resuming SPI water meter scanning...\n");
    printf("-------------------------\n");

    reading_locked_waiting_for_action = 0;
    morse_done_waiting_for_key1 = 0;

    valid_5digit_available = 0;
    current_5digit_reading[0] = '\0';

    /*
     * Clear this so the same reading can be detected again
     * after timeout/resume.
     */
    last_printed_reading[0] = '\0';

    reset_reading_buffer();

    spi_byte_ready = 0;
    spi_next_byte_needed = 0;
    spi_running = 1;

    spi_flush_rx();

    spi_start_one_byte_transfer(0xFF);
}

int action_window_expired(void)
{
    if (!reading_locked_waiting_for_action) {
        return 0;
    }

    /*
     * Important new behavior:
     * After KEY0 has played Morse, do NOT timeout.
     * Wait indefinitely for KEY1.
     */
    if (morse_done_waiting_for_key1) {
        return 0;
    }

    if (action_window_uses_ticks) {
        alt_u32 now = alt_nticks();
        alt_u32 ticks_per_second = alt_ticks_per_second();
        alt_u32 timeout_ticks = ticks_per_second * ACTION_WINDOW_SECONDS;

        if ((alt_u32)(now - action_window_start_ticks) >= timeout_ticks) {
            return 1;
        }
    }
    else {
        action_window_fallback_counter++;

        if (action_window_fallback_counter >= ACTION_WINDOW_FALLBACK_LOOPS) {
            return 1;
        }
    }

    return 0;
}

void handle_action_window_timeout(void)
{
    printf("3-second window expired.\n");
    printf("Discarding locked reading: %s\n", current_5digit_reading);
    printf("-------------------------\n");

    resume_spi_reading_after_action_window();
}

/* ---------------- SPI interrupt service routine ---------------- */
void spi_isr(void* context, alt_u32 id)
{
    uint8_t rx;

    while (READ_REG(SPI_STATUS_ADDR) & RRDY) {

        rx = (uint8_t)(READ_REG(SPI_RXDATA_ADDR) & 0xFF);

        short_delay(SPI_SS_DELAY);
        WRITE_REG(SPI_SS_ADDR, 0x1);

        if (reading_locked_waiting_for_action) {
            latest_spi_byte = 0;
            spi_byte_ready = 0;
            spi_next_byte_needed = 0;
            return;
        }

        latest_spi_byte = rx;
        spi_byte_ready = 1;

        if (spi_running) {
            spi_next_byte_needed = 1;
        }
    }
}

/* ---------------- Enable SPI interrupt ---------------- */
//void spi_interrupt_init(void)
//{
//    spi_flush_rx();
//
//    alt_ic_isr_register(
//        SPI_0_IRQ_INTERRUPT_CONTROLLER_ID,
//        SPI_0_IRQ,
//        spi_isr,
//        NULL,
//        NULL
//    );
//
//    WRITE_REG(SPI_CONTROL_ADDR, SPI_CONTROL_RRDY_INT_ENABLE);
//}

void spi_interrupt_init(void)
{
    spi_flush_rx();

    alt_irq_register(
        SPI_0_IRQ,
        NULL,
		spi_isr
    );

    WRITE_REG(SPI_CONTROL_ADDR, SPI_CONTROL_RRDY_INT_ENABLE);
}

/* ---------------- Process complete water meter reading ---------------- */
void process_complete_reading(const char *reading, alt_mutex_dev* mutex)
{
    if (reading_locked_waiting_for_action) {
        return;
    }

    if (strcmp(reading, last_printed_reading) == 0) {
        return;
    }

    strcpy(last_printed_reading, reading);

//    display_reading_on_leds(reading);

    if (reading[0] == NO_READING_BYTE && reading[1] == '\0') {
        printf("Water meter: NO DETECTION\n");
//        speaker_off();
        return;
    }

//    reading=reading&0x1F;

    printf("Water meter reading: %s\n", reading);

    if (is_valid_5digit_reading(reading)) {

    	strncpy(current_5digit_reading, reading,5);
        valid_5digit_available = 1;
        reading_locked_waiting_for_action = 1;
        morse_done_waiting_for_key1 = 0;

        printf("Valid 5-digit reading locked: %s\n", current_5digit_reading);
        printf("3-second action window started.\n");
        printf("KEY0 = play Morse code, then wait for KEY1.\n");
        printf("KEY1 = store reading to SDRAM and resume scanning.\n");
        printf("-------------------------\n");

//        beep_short();
//        generate_text_image_to_sdram(current_5digit_reading);
        uint32_t start_time, end_time;
        start_time = IORD_32DIRECT(timer, 0);


        clear_sdram_image(0x0);            // wipe SDRAM frame buffer to black
		generate_text_image_to_sdram(current_5digit_reading);    // build the text + background in SDRAM
		display_sdram_image();             // push every pixel from SDRAM to the VGA pixel buffer

		end_time = IORD_32DIRECT(timer, 0);
		printf("VGA Time(us): %u\n\n",(unsigned int)(end_time-start_time));
//		send_to_core1_int(0x30,1);


		altera_avalon_mutex_lock(mutex, 2);
		IOWR_16DIRECT(reading,0x30,reading);
		altera_avalon_mutex_unlock(mutex);

		usleep(10000);

//        stop_spi_reading_for_action_window();
    }
    else {
        printf("Reading is not exactly 5 digits. Ignoring for KEY0/KEY1 actions.\n");
        printf("-------------------------\n");
    }
}

/* ---------------- Process one received SPI byte ---------------- */
void process_received_byte(alt_mutex_dev* mutex)
{

	uint8_t b = latest_spi_byte;

    if (reading_locked_waiting_for_action) {
        reset_reading_buffer();
        return;
    }

#if DEBUG_SPI_BYTES
    printf("SPI byte received: 0x%02X", b);

    if (b >= 32 && b <= 126) {
        printf(" '%c'", b);
    }

    printf("\n");
#endif

    if (b == IDLE_SPI_BYTE) {
        return;
    }

    if (b == FRAME_END) {

        reading_buffer[reading_index] = '\0';

        if (reading_index > 0) {
            process_complete_reading(reading_buffer,mutex);
        }

        reset_reading_buffer();
        return;
    }

    if (b == CARRIAGE_RETURN) {
        return;
    }

    if ((b >= '0' && b <= '9') || b == NO_READING_BYTE) {

        if (reading_index < MAX_READING_LEN - 1) {
            reading_buffer[reading_index++] = (char)b;
            reading_buffer[reading_index] = '\0';
        }
        else {
            printf("Reading buffer overflow. Resetting frame.\n");
            reset_reading_buffer();
        }

        return;
    }

    printf("Unknown SPI byte: 0x%02X\n", b);

    reset_reading_buffer();
}

/* ---------------- Start continuous water meter reading ---------------- */
void spi_start_water_meter_reading(void)
{
//    printf("Starting live SPI water meter reading...\n");
//    printf("Expected frame from ESP32: digits followed by newline, e.g. 02012\\n\n");
//    printf("No detection frame: N\\n\n");
//    printf("Idle SPI byte 0xFF will be ignored.\n");
//    printf("-------------------------\n");

    latest_spi_byte = 0;

    spi_byte_ready = 0;
    spi_next_byte_needed = 0;
    spi_running = 1;

    reading_locked_waiting_for_action = 0;
    morse_done_waiting_for_key1 = 0;

    reset_reading_buffer();
    last_printed_reading[0] = '\0';

    current_5digit_reading[0] = '\0';
    valid_5digit_available = 0;

//    speaker_off();
//    all_morse_leds_off();
//    WRITE_REG(OUTPORT_LED_ADDR, 0x00);

    spi_flush_rx();

    spi_start_one_byte_transfer(0xFF);
}

/* ---------------- Key action handlers ---------------- */


void handle_key1_store(void)
{
    printf("\nKEY1 PRESSED\n");

    if (!valid_5digit_available) {
        printf("No valid 5-digit reading available to store.\n");
        printf("-------------------------\n");
        return;
    }

    printf("SDRAM store selected for locked reading: %s\n", current_5digit_reading);

    store_current_reading_to_sdram();

    print_reading_from_sdram();

    printf("Scanning will now resume.\n");
    printf("-------------------------\n");

    resume_spi_reading_after_action_window();
}



/* ---------------- Emergency stop ---------------- */

#define TILT_THRESHOLD_DEG   60     // beyond this = dangerous tilt
#define Z_UPSIDE_DOWN_THRESH -200   // depends on your sensor scaling
volatile int emergency_stop = 0;

void check_emergency_condition(int pitch, int roll, int z)
{
    if (abs(pitch) > TILT_THRESHOLD_DEG ||
        abs(roll)  > TILT_THRESHOLD_DEG ||
        z < Z_UPSIDE_DOWN_THRESH)
    {
        emergency_stop = 1;
    }
    else{
    	emergency_stop = 0;
    }
}

void handle_emergency_stop(alt_mutex_dev* mutex)
{
    // Disable SPI activity
	spi_running = 0;
    spi_next_byte_needed = 0;
    *SPI_SS = 0x1;

    printf("!!! EMERGENCY STOP !!!");
    // Turn off external outputs (via shared SDRAM flags)
    altera_avalon_mutex_lock(mutex, 2);


	  IOWR_16DIRECT(Shared_Sdram_Flag, 0x18,0);
	  IOWR_16DIRECT(Shared_Sdram_Flag, 0x1C,0);
	  IOWR_16DIRECT(Shared_Sdram_Flag, 0x20,0);
	  IOWR_16DIRECT(Shared_Sdram_Flag, 0x24,1);

    altera_avalon_mutex_unlock(mutex);

    // Infinite halt (latched safety)
    while (1);
}

#define TRDY 0x40
#define RRDY 0x80

int pitch_deg;
int roll_deg;
const char *ori;

unsigned char hex_buf[6] = {0};

/*Accelerometer INIT*/
alt_up_accelerometer_spi_dev *accel;
alt_32 x_axis;
alt_32 y_axis;
alt_32 z_axis;

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2

/* Prints "Hello World" and sleeps for three seconds */
void task1(void* pdata)
{
    int curr_key0;
    int curr_key1;

    alt_mutex_dev* mutex=altera_avalon_mutex_open("/dev/mutex");
    accel = alt_up_accelerometer_spi_open_dev("/dev/accelerometer_spi_0");

    for (int i = 0; i < 6; i++) hex_buf[i] = 0xFF;
    	hex_refresh();

	float pitch=0.0f;
	float roll=0.0f;
	float z=0.0f;

//	clear_sdram_image(0x0);            // wipe SDRAM frame buffer to black
//	generate_text_image_to_sdram("Start up");    // build the text + background in SDRAM
//	display_sdram_image();             // push every pixel from SDRAM to the VGA pixel buffer

//	alt_printf("Startup image drawn.\n");
	int swBase=0;
	int sw1;
	int sw2;
	int keyBase=0;
  while (1)
  { 
	  altera_avalon_mutex_lock(mutex, 2);
	  		swBase=IORD_16DIRECT(Shared_Sdram_Flag,0x10);
	  		keyBase=IORD_16DIRECT(Shared_Sdram_Flag,0x14);
	      	altera_avalon_mutex_unlock(mutex);

	  //    	int swBase=0x00;
	  //    	int keyBase=0x00;

	  		sw1 = swBase & 0x1;
	  		sw2 = swBase & 0x2;
	  		curr_key0 = keyBase & 0x1;
	  		curr_key1 = (keyBase & 0x2);

	  //		usleep(5000);
	  //		printf("0:%u\n",curr_key0);
	  //		usleep(5000);
	  //		printf("1:%u\n",curr_key1);

	  		if (sw1 == 1)
	  		{
	  			accelerometer_main(&pitch, &roll, &z);

	  			check_emergency_condition((int)pitch, (int)roll, (int)z);

	  			if (emergency_stop)
	  			{
	  				handle_emergency_stop(mutex);
	  			}
	  		}

	  		else if (sw2 == 2)
	  			{
	  			if (current_5digit_reading[0] != '\0') {
	  				hex_scroll_string(current_5digit_reading);
	  				}
	  			}

	  		else
	  		{
	  			for (int i = 0; i < 6; i++)
	  				hex_buf[i] = 0xFF;

	  			hex_refresh();
	  				}



	  		if ((curr_key1 == 2)&&(current_5digit_reading[0] != '\0'))
	  		{
	  			uint16_t numeric_value = (uint16_t)atoi(current_5digit_reading);
	  			handle_key1_store();
	  			altera_avalon_mutex_lock(mutex, 2);
	  			IOWR_16DIRECT(Shared_Sdram_Flag, 0x30,current_5digit_reading);
	  			altera_avalon_mutex_unlock(mutex);

	  		}
    OSTimeDlyHMSM(0, 0, 3, 0);
  }
}
/* Prints "Hello World" and sleeps for three seconds */
void task2(void* pdata)
{

    WRITE_REG(SPI_SS_BASE, 0x1);


    spi_flush_rx();
    spi_interrupt_init();

//    alt_printf("============================\n");
//
//    alt_printf("LIVE SPI WATER METER READER + SDRAM\n");
//
//    alt_printf("FPGA = SPI master\n");
//
//    alt_printf("ESP32 = SPI slave\n");
//
//    alt_printf("ESP32 sends ASCII frame:\n");
//    alt_printf("Example: 02012\\n\n");
//    alt_printf("No detection: N\\n\n");
//    alt_printf("Frame end byte is newline: 0x0A\n");
//    alt_printf("Idle byte 0xFF is ignored\n");
//    alt_printf("----------------------------\n");
//    alt_printf("When a valid 5-digit reading is detected:\n");
//    alt_printf("Detection pauses for 3 seconds.\n");
//    alt_printf("KEY0 = play locked reading as Morse code, then wait for KEY1\n");
//    alt_printf("KEY1 = store locked reading to SDRAM and resume scanning\n");
//    alt_printf("If no key is pressed in 3 seconds, scanning resumes.\n");
//    alt_printf("============================\n");

    spi_start_water_meter_reading();
  while (1)
  { 
	  if (spi_byte_ready) {
	          	uint32_t start_time, end_time;
	          	start_time = IORD_32DIRECT(timer, 0);

	          	spi_byte_ready = 0;
	          	process_received_byte(mutex);

	          	end_time = IORD_32DIRECT(timer, 0);
	              process_received_byte(mutex);
	  //            printf("spi Time(us): %u\n\n",(unsigned int)(end_time-start_time));

	          }

	          /*
	           * Request next byte from ESP32 only while SPI is running
	           * and the current reading is not locked.
	           */
	          if (!reading_locked_waiting_for_action &&
	              spi_running &&
	              spi_next_byte_needed) {

	              spi_next_byte_needed = 0;

	              short_delay(ESP32_QUEUE_DELAY);

	              spi_start_one_byte_transfer(0xFF);
	          }

	          if (action_window_expired()) {
	              handle_action_window_timeout();
	          }

	          short_delay(MAIN_LOOP_DELAY);
	          printf("Hello from task2\n");
    OSTimeDlyHMSM(0, 0, 3, 0);
  }
}
/* The main function creates two task and starts multi-tasking */
int main(void)
{

  OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);
              
               
  OSTaskCreateExt(task2,
                  NULL,
                  (void *)&task2_stk[TASK_STACKSIZE-1],
                  TASK2_PRIORITY,
                  TASK2_PRIORITY,
                  task2_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);
  OSStart();
  return 0;
}
