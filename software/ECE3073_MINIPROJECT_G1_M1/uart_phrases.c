#include "uart_phrases.h"
#include "hex.h"
#include "interrupt_handler.h"
#include <stdint.h>
#include <string.h>

#define MAX_PHRASE_LEN 32

extern volatile int *KEYS;

volatile int phrase_mode_initialized = 0;
volatile int phrase1_ready = 0;
volatile int phrase2_ready = 0;
char phrase1[MAX_PHRASE_LEN];
char phrase2[MAX_PHRASE_LEN];

/* Hardcoded phrases - edit these strings directly */
#define PHRASE1_TEXT  "Hello World"
#define PHRASE2_TEXT  "ECE3073 Group1"

void user_phrases_mode(void)
{
    if (!phrase_mode_initialized)
    {
        clear_phrases();

        strncpy(phrase1, PHRASE1_TEXT, MAX_PHRASE_LEN - 1);
        phrase1[MAX_PHRASE_LEN - 1] = '\0';
        phrase1_ready = 1;

        strncpy(phrase2, PHRASE2_TEXT, MAX_PHRASE_LEN - 1);
        phrase2[MAX_PHRASE_LEN - 1] = '\0';
        phrase2_ready = 1;

        uart_puts("USER PHRASES MODE\r\n");
        uart_puts("KEY0: "); uart_puts(phrase1); uart_puts("\r\n");
        uart_puts("KEY1: "); uart_puts(phrase2); uart_puts("\r\n");

        phrase_mode_initialized = 1;
    }

    handle_phrase_buttons();
}

void user_phrases_reset(void)
{
    clear_phrases();
    phrase_mode_initialized = 0;
}

void clear_phrases(void)
{
    int i;

    for (i = 0; i < MAX_PHRASE_LEN; i++)
    {
        phrase1[i] = '\0';
        phrase2[i] = '\0';
    }

    phrase1_ready = 0;
    phrase2_ready = 0;
}

void read_line(char *buf, int max_len)
{
    int i = 0;
    char c;

    while (i < max_len - 1)
    {
        c = uart_getchar();   /* receive from UART_0 */

        if (c == '\r' || c == '\n')
            break;

        buf[i++] = c;
        uart_putchar(c);      /* echo back on UART_0 */
    }

    buf[i] = '\0';
}

void handle_phrase_buttons(void)
{
    int keys = *KEYS;

    if ((keys & 0x1) == 0)   /* active-low KEY0 pressed */
    {
        if (phrase1_ready)
        {
            uart_puts(phrase1);
            uart_puts("\r\n");
            hex_test_alpha_scrolling(phrase1);
        }
    }

    if ((keys & 0x2) == 0)   /* active-low KEY1 pressed */
    {
        if (phrase2_ready)
        {
            uart_puts(phrase2);
            uart_puts("\r\n");
            hex_test_alpha_scrolling(phrase2);
        }
    }
}
