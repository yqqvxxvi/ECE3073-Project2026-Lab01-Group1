#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <system.h>
#include <io.h>
#include <sys/alt_irq.h>

#include "sdram.h"

/* ---------------- SPI registers ---------------- */
volatile int *SPI_RxData  = (int *)0x08009000;
volatile int *SPI_TxData  = (int *)0x08009004;
volatile int *SPI_Status  = (int *)0x08009008;
volatile int *SPI_Control = (int *)0x0800900C;

/* ---------------- PIO registers ---------------- */
volatile int *SPI_SS      = (int *)0x080090c0;
volatile int *OutPort_LED = (int *)0x080090e0;
volatile int *green_LED   = (int *)0x080090b0;
volatile int *speaker     = (int *)0x080090a0;

volatile int *KEY0        = (int *)0x08009090;
volatile int *KEY1        = (int *)0x080090f0;

/* ---------------- SPI bits ---------------- */
#define TRDY 0x40
#define RRDY 0x80

/*
 * Avalon SPI control register interrupt-enable bit.
 * This enables interrupt when receive data is ready.
 */
#define SPI_CONTROL_RRDY_INT_ENABLE 0x80

/* ---------------- SDRAM layout ---------------- */
#define SDRAM_MSG_OFFSET   0
#define MAX_MESSAGE_LEN    64

/* ---------------- Message markers ---------------- */
#define START_MARKER '<'
#define END_MARKER   '>'

/* ---------------- Delay tuning ---------------- */
#define SPI_SS_DELAY          1000
#define ESP32_QUEUE_DELAY     5000
#define KEY_DEBOUNCE_DELAY    30000
#define MAIN_LOOP_DELAY       1000

/* ---------------- Morse buzzer tuning ---------------- */

/*
 * If your speaker turns ON when writing 1, keep this.
 *
 * If your speaker logic is inverted, change this:
 *
 * #define SPEAKER_ON_VALUE   0
 * #define SPEAKER_OFF_VALUE  1
 */
#define SPEAKER_ON_VALUE   1
#define SPEAKER_OFF_VALUE  0

/*
 * Controls silent gap length between dots/dashes/letters.
 *
 * Increase = slower Morse gaps.
 * Decrease = faster Morse gaps.
 */
#define MORSE_UNIT_DELAY   60000

/*
 * Controls beep duration.
 *
 * Dot  = 1 x MORSE_DOT_TOGGLES
 * Dash = 3 x MORSE_DOT_TOGGLES
 *
 * If H is still too long, reduce this to 60.
 */
#define MORSE_DOT_TOGGLES  60

/*
 * Controls pitch of buzzer tone.
 *
 * Smaller = higher pitch.
 * Larger = lower pitch.
 */
#define TONE_HALF_PERIOD   120

/* ---------------- Global interrupt state ---------------- */
volatile char spi_message[MAX_MESSAGE_LEN];

volatile int spi_index = 0;
volatile int spi_receiving = 0;
volatile int spi_inside_message = 0;
volatile int spi_message_ready = 0;
volatile int spi_overflow = 0;
volatile int spi_next_byte_needed = 0;

/* ---------------- Helper delays ---------------- */
void short_delay(volatile int count)
{
    for (volatile int i = 0; i < count; i++);
}

void tone_delay(volatile int count)
{
    for (volatile int i = 0; i < count; i++);
}

/* ---------------- Speaker helpers ---------------- */

void speaker_on(void)
{
    *speaker = SPEAKER_ON_VALUE;
}

void speaker_off(void)
{
    *speaker = SPEAKER_OFF_VALUE;
}

void beep_short(void)
{
    for (int i = 0; i < 30; i++) {
        speaker_on();
        tone_delay(60);
        speaker_off();
        tone_delay(60);
    }

    speaker_off();
}

/* ---------------- Morse buzzer functions ---------------- */

/*
 * This creates the actual buzzing sound.
 *
 * Old problem:
 * The previous code used MORSE_UNIT_TIME = 60000 as the number of tone cycles.
 * That made one dot extremely long.
 *
 * New version:
 * A dot only uses MORSE_DOT_TOGGLES cycles.
 */
void morse_tone_for_units(int units)
{
    int total_toggles = units * MORSE_DOT_TOGGLES;

    for (int i = 0; i < total_toggles; i++) {
        speaker_on();
        tone_delay(TONE_HALF_PERIOD);

        speaker_off();
        tone_delay(TONE_HALF_PERIOD);
    }

    speaker_off();
}

/*
 * Morse timing:
 *
 * dot  = 1 unit
 * dash = 3 units
 *
 * gap between dots/dashes inside a letter = 1 unit
 * gap between letters = 3 units
 * gap between words = 7 units
 */

void morse_dot(void)
{
    morse_tone_for_units(1);

    /*
     * 1 unit gap after each dot/dash.
     */
    short_delay(MORSE_UNIT_DELAY);
}

void morse_dash(void)
{
    morse_tone_for_units(3);

    /*
     * 1 unit gap after each dot/dash.
     */
    short_delay(MORSE_UNIT_DELAY);
}

const char *morse_lookup(char c)
{
    switch (c) {
        case 'A': case 'a': return ".-";
        case 'B': case 'b': return "-...";
        case 'C': case 'c': return "-.-.";
        case 'D': case 'd': return "-..";
        case 'E': case 'e': return ".";
        case 'F': case 'f': return "..-.";
        case 'G': case 'g': return "--.";
        case 'H': case 'h': return "....";
        case 'I': case 'i': return "..";
        case 'J': case 'j': return ".---";
        case 'K': case 'k': return "-.-";
        case 'L': case 'l': return ".-..";
        case 'M': case 'm': return "--";
        case 'N': case 'n': return "-.";
        case 'O': case 'o': return "---";
        case 'P': case 'p': return ".--.";
        case 'Q': case 'q': return "--.-";
        case 'R': case 'r': return ".-.";
        case 'S': case 's': return "...";
        case 'T': case 't': return "-";
        case 'U': case 'u': return "..-";
        case 'V': case 'v': return "...-";
        case 'W': case 'w': return ".--";
        case 'X': case 'x': return "-..-";
        case 'Y': case 'y': return "-.--";
        case 'Z': case 'z': return "--..";

        case '0': return "-----";
        case '1': return ".----";
        case '2': return "..---";
        case '3': return "...--";
        case '4': return "....-";
        case '5': return ".....";
        case '6': return "-....";
        case '7': return "--...";
        case '8': return "---..";
        case '9': return "----.";

        default: return "";
    }
}

void play_morse_char(char c)
{
    const char *code = morse_lookup(c);

    for (int i = 0; code[i] != '\0'; i++) {
        if (code[i] == '.') {
            morse_dot();
        } else if (code[i] == '-') {
            morse_dash();
        }
    }

    /*
     * We already waited 1 unit after the final dot/dash.
     * Letter gap should be 3 units total.
     * So add 2 more units.
     */
    short_delay(MORSE_UNIT_DELAY * 2);
}

void play_morse_string(const char *msg)
{
    printf("Playing Morse: ");

    for (int i = 0; msg[i] != '\0'; i++) {
        const char *code = morse_lookup(msg[i]);

        if (msg[i] == ' ') {
            printf(" / ");

            /*
             * Word gap should be 7 units.
             */
            short_delay(MORSE_UNIT_DELAY * 7);
        } else {
            printf("%c=%s ", msg[i], code);
            play_morse_char(msg[i]);
        }
    }

    printf("\n");
    speaker_off();
}

/*
 * Clear any old received bytes from the SPI receive register.
 */
void spi_flush_rx(void)
{
    while (*SPI_Status & RRDY) {
        (void)(uint8_t)(*SPI_RxData & 0xFF);
    }
}

/*
 * Start one SPI byte transaction.
 *
 * This function does NOT wait for RRDY.
 * It only starts the transfer.
 * The interrupt will happen when the received byte is ready.
 */
void spi_start_one_byte_transfer(uint8_t tx)
{
    *SPI_SS = 0x0;
    short_delay(SPI_SS_DELAY);

    while (!(*SPI_Status & TRDY));

    *SPI_TxData = tx;
}

/*
 * SPI interrupt service routine.
 *
 * This runs automatically when RRDY becomes set.
 *
 * Important:
 * Do not printf here.
 * Do not write to SDRAM here.
 * Do not beep here.
 */
void spi_isr(void *context)
{
    uint8_t rx;

    while (*SPI_Status & RRDY) {

        rx = (uint8_t)(*SPI_RxData & 0xFF);

        /*
         * End this one-byte SPI transaction.
         */
        short_delay(SPI_SS_DELAY);
        *SPI_SS = 0x1;

        if (!spi_receiving) {
            return;
        }

        /*
         * Start marker found.
         * From now on, begin storing bytes.
         *
         * Example:
         * <HELLO>
         * ^
         */
        if (rx == START_MARKER) {
            spi_inside_message = 1;
            spi_index = 0;
            spi_message[0] = '\0';

            spi_next_byte_needed = 1;
            return;
        }

        /*
         * If we have not seen '<' yet, ignore this byte.
         */
        if (!spi_inside_message) {
            spi_next_byte_needed = 1;
            return;
        }

        /*
         * End marker found.
         *
         * Example:
         * <HELLO>
         *       ^
         */
        if (rx == END_MARKER || rx == '\0') {
            spi_message[spi_index] = '\0';

            spi_inside_message = 0;
            spi_receiving = 0;
            spi_message_ready = 1;
            spi_next_byte_needed = 0;

            return;
        }

        /*
         * Normal data byte between '<' and '>'.
         */
        if (spi_index < MAX_MESSAGE_LEN - 1) {
            spi_message[spi_index] = (char)rx;
            spi_index++;

            /*
             * Tell main loop to start the next SPI byte transfer.
             * Do not start the next transfer directly here.
             * ESP32 needs time to queue its next byte.
             */
            spi_next_byte_needed = 1;
        } else {
            spi_message[MAX_MESSAGE_LEN - 1] = '\0';

            spi_inside_message = 0;
            spi_receiving = 0;
            spi_overflow = 1;
            spi_next_byte_needed = 0;

            return;
        }
    }
}

/*
 * Enable SPI interrupt in both CPU and SPI peripheral.
 */
void spi_interrupt_init(void)
{
    /*
     * Make sure no stale byte is waiting before enabling interrupts.
     */
    spi_flush_rx();

    /*
     * Register ISR for SPI_0_IRQ.
     */
    alt_ic_isr_register(
        SPI_0_IRQ_INTERRUPT_CONTROLLER_ID,
        SPI_0_IRQ,
        spi_isr,
        NULL,
        NULL
    );

    /*
     * Enable receive-ready interrupt from SPI peripheral.
     */
    *SPI_Control = SPI_CONTROL_RRDY_INT_ENABLE;
}

/*
 * Start receiving one marked sentence using SPI interrupts.
 *
 * Expected incoming format:
 * <HELLO>
 *
 * Stored result:
 * HELLO
 */
void spi_begin_receive_sentence(void)
{
    memset((void *)spi_message, 0, MAX_MESSAGE_LEN);

    spi_index = 0;
    spi_receiving = 1;
    spi_inside_message = 0;
    spi_message_ready = 0;
    spi_overflow = 0;
    spi_next_byte_needed = 0;

    speaker_off();
    *green_LED   = 0x00;
    *OutPort_LED = 0x00;

    spi_flush_rx();

    printf("START SPI MARKED STRING RECEIVE USING INTERRUPTS\n");
    printf("Waiting for '<' start marker...\n");

    /*
     * Start the first byte transfer.
     * When the byte arrives, spi_isr() will run.
     */
    spi_start_one_byte_transfer(0xFF);
}

/*
 * Once the message is complete, store it into SDRAM,
 * then play the received sentence as Morse code.
 */
void store_completed_message_to_sdram(void)
{
    char local_message[MAX_MESSAGE_LEN];
    int len;

    memset(local_message, 0, sizeof(local_message));

    /*
     * Copy from volatile ISR buffer into normal local buffer.
     */
    for (int i = 0; i < MAX_MESSAGE_LEN; i++) {
        local_message[i] = spi_message[i];

        if (spi_message[i] == '\0') {
            break;
        }
    }

    len = strlen(local_message);

    sdram_write_array(SDRAM_MSG_OFFSET, (uint8_t *)local_message, len + 1);

    printf("Sentence receive complete\n");
    printf("Received sentence: %s\n", local_message);
    printf("Stored %d characters into SDRAM\n", len);
    printf("-------------------------\n");

    *OutPort_LED = 0x01;
    *green_LED   = 0x01;

    /*
     * Convert received sentence to Morse and buzz it.
     *
     * Example:
     * HELLO = .... . .-.. .-.. ---
     */
    play_morse_string(local_message);

    speaker_off();
    *OutPort_LED = 0x00;
    *green_LED   = 0x00;
}

/*
 * Read stored sentence back from SDRAM and print it.
 */
void print_sentence_from_sdram(void)
{
    char message[MAX_MESSAGE_LEN];

    memset(message, 0, sizeof(message));
    sdram_read_string(SDRAM_MSG_OFFSET, message, sizeof(message));

    printf("Message from SDRAM: %s\n", message);
    printf("-------------------------\n");
}

int main(void)
{
    int prev_key0;
    int prev_key1;
    int curr_key0;
    int curr_key1;

    *SPI_SS      = 0x1;
    *OutPort_LED = 0x00;
    *green_LED   = 0x00;
    speaker_off();

    spi_flush_rx();
    spi_interrupt_init();

    prev_key0 = (*KEY0) & 0x1;
    prev_key1 = (*KEY1) & 0x1;

    printf("============================\n");
    printf("SPI MASTER READY - INTERRUPT VERSION WITH MARKERS + MORSE BUZZER\n");
    printf("ESP32 should send format: <HELLO>\n");
    printf("KEY0 = receive marked sentence into SDRAM using SPI interrupt\n");
    printf("KEY1 = print stored sentence from SDRAM\n");
    printf("After receive, speaker buzzes Morse version of the sentence\n");
    printf("============================\n");

    while (1) {

        curr_key0 = (*KEY0) & 0x1;
        curr_key1 = (*KEY1) & 0x1;

        /*
         * KEY0 active-low press.
         */
        if ((prev_key0 == 1) && (curr_key0 == 0)) {

            short_delay(KEY_DEBOUNCE_DELAY);

            if (!spi_receiving) {
                printf("\nKEY0 PRESSED\n");
                spi_begin_receive_sentence();
            } else {
                printf("SPI receive already in progress\n");
            }
        }

        /*
         * The ISR sets spi_next_byte_needed after receiving a byte.
         * Main waits briefly so ESP32 can queue the next character,
         * then starts the next SPI transfer.
         */
        if (spi_receiving && spi_next_byte_needed) {

            spi_next_byte_needed = 0;

            short_delay(ESP32_QUEUE_DELAY);

            spi_start_one_byte_transfer(0xFF);
        }

        /*
         * Message completed by ISR.
         * Do SDRAM write, printf, and Morse playback outside the ISR.
         */
        if (spi_message_ready) {

            spi_message_ready = 0;

            store_completed_message_to_sdram();

            printf("Receive successful\n");
        }

        /*
         * Overflow handled outside ISR.
         */
        if (spi_overflow) {

            spi_overflow = 0;

            printf("Buffer overflow, sentence truncated\n");
            printf("SPI receive failed or truncated\n");

            speaker_off();
            *green_LED = 0x00;
            *SPI_SS    = 0x1;
        }

        /*
         * KEY1 active-low press.
         */
        if ((prev_key1 == 1) && (curr_key1 == 0)) {

            short_delay(KEY_DEBOUNCE_DELAY);

            printf("\nKEY1 PRESSED\n");

            print_sentence_from_sdram();
        }

        prev_key0 = curr_key0;
        prev_key1 = curr_key1;

        short_delay(MAIN_LOOP_DELAY);
    }

    return 0;
}
