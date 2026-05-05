#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <system.h>
#include <io.h>
#include <sys/alt_irq.h>
#include <sys/alt_alarm.h>

#include "sdram.h"

/* ---------------- SPI registers ---------------- */
volatile int *SPI_RxData  = (int *)0x08009000;
volatile int *SPI_TxData  = (int *)0x08009004;
volatile int *SPI_Status  = (int *)0x08009008;
volatile int *SPI_Control = (int *)0x0800900C;

/* ---------------- Updated PIO registers ---------------- */
volatile int *SPI_SS      = (int *)0x08009100;
volatile int *OutPort_LED = (int *)0x08009120;
volatile int *green_LED   = (int *)0x080090f0;
volatile int *speaker     = (int *)0x080090e0;
volatile int *yellow_LED  = (int *)0x080090c0;
volatile int *red_LED     = (int *)0x080090b0;

volatile int *KEY0        = (int *)0x080090d0;
volatile int *KEY1        = (int *)0x08009130;

/* ---------------- SPI bits ---------------- */
#define TRDY 0x40
#define RRDY 0x80

#define SPI_CONTROL_RRDY_INT_ENABLE 0x80

/* ---------------- SDRAM layout ---------------- */
#define SDRAM_READING_OFFSET  0
#define STORED_READING_LEN    6     /* 5 digits + '\0' */

/* ---------------- Timing ---------------- */
#define SPI_SS_DELAY          1000
#define ESP32_QUEUE_DELAY     1000
#define MAIN_LOOP_DELAY       100
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

#define MORSE_UNIT_DELAY      60000
#define MORSE_DOT_TOGGLES     60
#define TONE_HALF_PERIOD      120

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

/* ---------------- LED helpers ---------------- */
void all_morse_leds_off(void)
{
    *red_LED    = LED_OFF_VALUE;
    *yellow_LED = LED_OFF_VALUE;
    *green_LED  = LED_OFF_VALUE;
}

void red_on_only(void)
{
    *red_LED    = LED_ON_VALUE;
    *yellow_LED = LED_OFF_VALUE;
    *green_LED  = LED_OFF_VALUE;
}

void yellow_on_only(void)
{
    *red_LED    = LED_OFF_VALUE;
    *yellow_LED = LED_ON_VALUE;
    *green_LED  = LED_OFF_VALUE;
}

void green_on_only(void)
{
    *red_LED    = LED_OFF_VALUE;
    *yellow_LED = LED_OFF_VALUE;
    *green_LED  = LED_ON_VALUE;
}

void beep_short(void)
{
    for (int i = 0; i < 15; i++) {
        speaker_on();
        tone_delay(50);
        speaker_off();
        tone_delay(50);
    }

    speaker_off();
}

/* ---------------- Morse buzzer functions ---------------- */

/*
 * led_type:
 * 0 = no special LED
 * 1 = red LED for dot
 * 2 = yellow LED for dash
 */
void morse_tone_for_units(int units, int led_type)
{
    int total_toggles = units * MORSE_DOT_TOGGLES;

    if (led_type == 1) {
        red_on_only();
    }
    else if (led_type == 2) {
        yellow_on_only();
    }
    else {
        all_morse_leds_off();
    }

    for (int i = 0; i < total_toggles; i++) {
        speaker_on();
        tone_delay(TONE_HALF_PERIOD);

        speaker_off();
        tone_delay(TONE_HALF_PERIOD);
    }

    speaker_off();
    all_morse_leds_off();
}

void morse_dot(void)
{
    morse_tone_for_units(1, 1);
    short_delay(MORSE_UNIT_DELAY);
}

void morse_dash(void)
{
    morse_tone_for_units(3, 2);
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
        }
        else if (code[i] == '-') {
            morse_dash();
        }
    }

    short_delay(MORSE_UNIT_DELAY * 2);
}

void play_morse_string(const char *msg)
{
    printf("Playing Morse for reading: %s\n", msg);
    printf("Morse: ");

    all_morse_leds_off();

    for (int i = 0; msg[i] != '\0'; i++) {
        const char *code = morse_lookup(msg[i]);

        if (msg[i] == ' ') {
            printf(" / ");
            short_delay(MORSE_UNIT_DELAY * 7);
        }
        else {
            printf("%c=%s ", msg[i], code);
            play_morse_char(msg[i]);
        }
    }

    printf("\n");

    speaker_off();

    green_on_only();
    short_delay(MORSE_UNIT_DELAY * 8);
    all_morse_leds_off();
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

    *green_LED = LED_ON_VALUE;
    *OutPort_LED = 0x0F;

    beep_short();

    *green_LED = LED_OFF_VALUE;
    *OutPort_LED = 0x00;
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
    if (strlen(reading) != REQUIRED_DIGITS) {
        return 0;
    }

    return is_all_digits(reading);
}

/* ---------------- SPI helpers ---------------- */
void spi_flush_rx(void)
{
    while (*SPI_Status & RRDY) {
        (void)(uint8_t)(*SPI_RxData & 0xFF);
    }
}

void spi_start_one_byte_transfer(uint8_t tx)
{
    *SPI_SS = 0x0;
    short_delay(SPI_SS_DELAY);

    while (!(*SPI_Status & TRDY));

    *SPI_TxData = tx;
}

/* ---------------- Lock / unlock scanning ---------------- */
void stop_spi_reading_for_action_window(void)
{
    spi_running = 0;
    spi_next_byte_needed = 0;
    spi_byte_ready = 0;

    *SPI_SS = 0x1;

    spi_flush_rx();

    action_window_start_ticks = alt_nticks();

    if (alt_ticks_per_second() > 0) {
        action_window_uses_ticks = 1;
    }
    else {
        action_window_uses_ticks = 0;
    }

    action_window_fallback_counter = 0;

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

/* ---------------- Display reading on output LEDs ---------------- */
void display_reading_on_leds(const char *reading)
{
    if (reading[0] == NO_READING_BYTE || reading[0] == '\0') {
        *OutPort_LED = 0x00;
        return;
    }

    int len = strlen(reading);
    char last_char = reading[len - 1];

    if (last_char >= '0' && last_char <= '9') {
        int last_digit = last_char - '0';
        *OutPort_LED = last_digit & 0x0F;
    }
    else {
        *OutPort_LED = 0x00;
    }
}

/* ---------------- SPI interrupt service routine ---------------- */
void spi_isr(void *context)
{
    uint8_t rx;

    while (*SPI_Status & RRDY) {

        rx = (uint8_t)(*SPI_RxData & 0xFF);

        short_delay(SPI_SS_DELAY);
        *SPI_SS = 0x1;

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
void spi_interrupt_init(void)
{
    spi_flush_rx();

    alt_ic_isr_register(
        SPI_0_IRQ_INTERRUPT_CONTROLLER_ID,
        SPI_0_IRQ,
        spi_isr,
        NULL,
        NULL
    );

    *SPI_Control = SPI_CONTROL_RRDY_INT_ENABLE;
}

/* ---------------- Process complete water meter reading ---------------- */
void process_complete_reading(const char *reading)
{
    if (reading_locked_waiting_for_action) {
        return;
    }

    if (strcmp(reading, last_printed_reading) == 0) {
        return;
    }

    strcpy(last_printed_reading, reading);

    display_reading_on_leds(reading);

    if (reading[0] == NO_READING_BYTE && reading[1] == '\0') {
        printf("Water meter: NO DETECTION\n");
        speaker_off();
        return;
    }

    printf("Water meter reading: %s\n", reading);

    if (is_valid_5digit_reading(reading)) {

        strcpy(current_5digit_reading, reading);
        valid_5digit_available = 1;
        reading_locked_waiting_for_action = 1;

        printf("Valid 5-digit reading locked: %s\n", current_5digit_reading);
        printf("3-second action window started.\n");
        printf("KEY0 = play Morse code.\n");
        printf("KEY1 = store reading to SDRAM.\n");
        printf("-------------------------\n");

        beep_short();

        stop_spi_reading_for_action_window();
    }
    else {
        printf("Reading is not exactly 5 digits. Ignoring for KEY0/KEY1 actions.\n");
        printf("-------------------------\n");
    }
}

/* ---------------- Process one received SPI byte ---------------- */
void process_received_byte(void)
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
            process_complete_reading(reading_buffer);
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
    printf("Starting live SPI water meter reading...\n");
    printf("Expected frame from ESP32: digits followed by newline, e.g. 02012\\n\n");
    printf("No detection frame: N\\n\n");
    printf("Idle SPI byte 0xFF will be ignored.\n");
    printf("-------------------------\n");

    latest_spi_byte = 0;

    spi_byte_ready = 0;
    spi_next_byte_needed = 0;
    spi_running = 1;

    reading_locked_waiting_for_action = 0;

    reset_reading_buffer();
    last_printed_reading[0] = '\0';

    current_5digit_reading[0] = '\0';
    valid_5digit_available = 0;

    speaker_off();
    all_morse_leds_off();
    *OutPort_LED = 0x00;

    spi_flush_rx();

    spi_start_one_byte_transfer(0xFF);
}

/* ---------------- Key action handlers ---------------- */
void handle_key0_morse(void)
{
    printf("\nKEY0 PRESSED\n");

    if (!valid_5digit_available) {
        printf("No valid 5-digit reading available for Morse playback.\n");
        printf("-------------------------\n");
        return;
    }

    printf("Morse playback selected for locked reading: %s\n", current_5digit_reading);
    printf("Dot = red LED, Dash = yellow LED, Finished = green LED\n");

    *OutPort_LED = 0x01;

    play_morse_string(current_5digit_reading);

    speaker_off();
    *OutPort_LED = 0x00;

    printf("Morse playback complete.\n");
    printf("Scanning will now resume.\n");
    printf("-------------------------\n");

    resume_spi_reading_after_action_window();
}

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

/* ---------------- Main ---------------- */
int main(void)
{
    int prev_key0;
    int prev_key1;
    int curr_key0;
    int curr_key1;

    *SPI_SS      = 0x1;
    *OutPort_LED = 0x00;
    speaker_off();
    all_morse_leds_off();

    spi_flush_rx();
    spi_interrupt_init();

    prev_key0 = (*KEY0) & 0x1;
    prev_key1 = (*KEY1) & 0x1;

    printf("============================\n");
    printf("LIVE SPI WATER METER READER + MORSE + SDRAM\n");
    printf("FPGA = SPI master\n");
    printf("ESP32 = SPI slave\n");
    printf("ESP32 sends ASCII frame:\n");
    printf("Example: 02012\\n\n");
    printf("No detection: N\\n\n");
    printf("Frame end byte is newline: 0x0A\n");
    printf("Idle byte 0xFF is ignored\n");
    printf("----------------------------\n");
    printf("When a valid 5-digit reading is detected:\n");
    printf("Detection pauses for 3 seconds.\n");
    printf("KEY0 = play locked reading as Morse code on buzzer\n");
    printf("      dot = red LED, dash = yellow LED, finished = green LED\n");
    printf("KEY1 = store locked reading to SDRAM\n");
    printf("If no key is pressed in 3 seconds, scanning resumes.\n");
    printf("============================\n");

    spi_start_water_meter_reading();

    while (1) {

        curr_key0 = (*KEY0) & 0x1;
        curr_key1 = (*KEY1) & 0x1;

        /*
         * Process completed SPI byte outside ISR.
         */
        if (spi_byte_ready) {
            spi_byte_ready = 0;
            process_received_byte();
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

        /*
         * KEY0 active-low press:
         * play locked valid 5-digit reading as Morse.
         */
        if ((prev_key0 == 1) && (curr_key0 == 0)) {

            short_delay(KEY_DEBOUNCE_DELAY);

            curr_key0 = (*KEY0) & 0x1;

            if (curr_key0 == 0) {
                handle_key0_morse();
            }
        }

        /*
         * KEY1 active-low press:
         * store locked valid 5-digit reading to SDRAM.
         */
        if ((prev_key1 == 1) && (curr_key1 == 0)) {

            short_delay(KEY_DEBOUNCE_DELAY);

            curr_key1 = (*KEY1) & 0x1;

            if (curr_key1 == 0) {
                handle_key1_store();
            }
        }

        /*
         * If no key is pressed within 3 seconds, discard the locked
         * reading and resume scanning.
         */
        if (action_window_expired()) {
            handle_action_window_timeout();
        }

        prev_key0 = curr_key0;
        prev_key1 = curr_key1;

        short_delay(MAIN_LOOP_DELAY);
    }

    return 0;
}
