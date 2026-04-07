#include "uart_phrases.h"
#include "hex.h"
#include "system.h"
#include "unistd.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_PHRASE_LEN 32

extern volatile int *KEYS;

volatile int phrase_mode_initialized = 0;
volatile int phrase1_ready = 0;
volatile int phrase2_ready = 0;
char phrase1[MAX_PHRASE_LEN];
char phrase2[MAX_PHRASE_LEN];

void user_phrases_mode(void)
{
    if (!phrase_mode_initialized)
    {
        clear_phrases();

        printf("USER PHRASES MODE\r\n");
        printf("Enter phrase 1: ");
        read_line(phrase1, MAX_PHRASE_LEN);
        printf("\r\nPhrase 1 saved: %s\r\n", phrase1);
        phrase1_ready = 1;

        printf("Enter phrase 2: ");
        read_line(phrase2, MAX_PHRASE_LEN);
        printf("\r\nPhrase 2 saved: %s\r\n", phrase2);
        phrase2_ready = 1;

        printf("Press KEY0 for phrase 1, KEY1 for phrase 2\r\n");
        printf("Leave this mode and come back to reset\r\n");

        phrase_mode_initialized = 1;
    }

    handle_phrase_buttons();
}

void user_phrases_reset(void)
{
    clear_phrases();
    phrase_mode_initialized = 0;
}

// helper
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
        c = getchar();

        if (c == '\r' || c == '\n')
        {
            break;
        }

        buf[i++] = c;
        putchar(c);   // echo typed character
    }

    buf[i] = '\0';
}

void handle_phrase_buttons(void)
{
    int keys = *KEYS;

    if ((keys & 0x1) == 0)   // if active-low KEY0 pressed
    {
        if (phrase1_ready)
        {
            hex_test_alpha_scrolling(phrase1);
        }
    }

    if ((keys & 0x2) == 0)   // if active-low KEY1 pressed
    {
        if (phrase2_ready)
        {
            hex_test_alpha_scrolling(phrase2);
        }
    }
}