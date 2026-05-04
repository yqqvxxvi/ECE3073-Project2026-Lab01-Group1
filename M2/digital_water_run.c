#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <system.h>
#include <io.h>
#include <sys/alt_irq.h>

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

/* ---------------- SPI bits ---------------- */
#define TRDY 0x40
#define RRDY 0x80

#define SPI_CONTROL_RRDY_INT_ENABLE 0x80

/* ---------------- Timing ---------------- */
#define SPI_SS_DELAY       1000
#define ESP32_QUEUE_DELAY  1000
#define MAIN_LOOP_DELAY    100

/* ---------------- Water meter frame ---------------- */
#define FRAME_END          '\n'    /* 0x0A */
#define CARRIAGE_RETURN    '\r'    /* 0x0D */
#define NO_READING_BYTE    'N'
#define IDLE_SPI_BYTE      0xFF
#define MAX_READING_LEN    16

/*
 * Set this to 1 if you want to see every SPI byte received.
 * Keep it 0 normally, because printing every byte can slow the system.
 */
#define DEBUG_SPI_BYTES    0

/* ---------------- Global SPI state ---------------- */
volatile uint8_t latest_spi_byte = 0;

volatile int spi_running = 0;
volatile int spi_byte_ready = 0;
volatile int spi_next_byte_needed = 0;

/* ---------------- Reading buffer ---------------- */
char reading_buffer[MAX_READING_LEN];
int reading_index = 0;

char last_printed_reading[MAX_READING_LEN] = "";

/* ---------------- Helper delays ---------------- */
void short_delay(volatile int count)
{
    for (volatile int i = 0; i < count; i++);
}

void tone_delay(volatile int count)
{
    for (volatile int i = 0; i < count; i++);
}

/* ---------------- Simple beep ---------------- */
void beep_short(void)
{
    for (int i = 0; i < 15; i++) {
        *speaker = 0x01;
        tone_delay(50);
        *speaker = 0x00;
        tone_delay(50);
    }
}

/* ---------------- Flush stale SPI RX bytes ---------------- */
void spi_flush_rx(void)
{
    while (*SPI_Status & RRDY) {
        (void)(uint8_t)(*SPI_RxData & 0xFF);
    }
}

/* ---------------- Start one SPI byte transfer ---------------- */
void spi_start_one_byte_transfer(uint8_t tx)
{
    *SPI_SS = 0x0;
    short_delay(SPI_SS_DELAY);

    while (!(*SPI_Status & TRDY));

    *SPI_TxData = tx;
}

/* ---------------- Display reading on LEDs ---------------- */
void display_reading_on_leds(const char *reading)
{
    /*
     * Simple display logic:
     * - green LED ON if valid reading received
     * - output LEDs show last digit as binary
     */

    if (reading[0] == NO_READING_BYTE || reading[0] == '\0') {
        *green_LED = 0x00;
        *OutPort_LED = 0x00;
        return;
    }

    int len = strlen(reading);
    char last_char = reading[len - 1];

    if (last_char >= '0' && last_char <= '9') {
        int last_digit = last_char - '0';

        *green_LED = 0x01;
        *OutPort_LED = last_digit & 0x0F;
    }
    else {
        *green_LED = 0x00;
        *OutPort_LED = 0x00;
    }
}

/* ---------------- SPI interrupt service routine ---------------- */
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

/* ---------------- Process complete reading ---------------- */
void process_complete_reading(const char *reading)
{
    /*
     * Avoid printing the same reading repeatedly.
     */
    if (strcmp(reading, last_printed_reading) == 0) {
        return;
    }

    strcpy(last_printed_reading, reading);

    display_reading_on_leds(reading);

    if (reading[0] == NO_READING_BYTE && reading[1] == '\0') {
        printf("Water meter: NO DETECTION\n");
        *speaker = 0x00;
    }
    else {
        printf("Water meter reading: %s\n", reading);
        beep_short();
    }
}

/* ---------------- Reset partial frame ---------------- */
void reset_reading_buffer(void)
{
    reading_index = 0;
    reading_buffer[0] = '\0';
}

/* ---------------- Process one received SPI byte ---------------- */
void process_received_byte(void)
{
    uint8_t b = latest_spi_byte;

#if DEBUG_SPI_BYTES
    printf("SPI byte received: 0x%02X", b);

    if (b >= 32 && b <= 126) {
        printf(" '%c'", b);
    }

    printf("\n");
#endif

    /*
     * Ignore idle/dummy byte.
     *
     * 0xFF often appears when the ESP32 has no useful byte ready,
     * or when the SPI bus returns the dummy/default value.
     *
     * Do not print this, otherwise it floods the console.
     */
    if (b == IDLE_SPI_BYTE) {
        return;
    }

    /*
     * 0x0A = '\n'
     * This marks the end of one full water meter frame.
     *
     * Example frame:
     * '0' '2' '0' '1' '2' '\n'
     */
    if (b == FRAME_END) {

        reading_buffer[reading_index] = '\0';

        if (reading_index > 0) {
            process_complete_reading(reading_buffer);
        }
        else {
            /*
             * Empty frame received. Do not treat this as an error.
             */
#if DEBUG_SPI_BYTES
            printf("Empty frame received.\n");
#endif
        }

        reset_reading_buffer();
        return;
    }

    /*
     * Ignore carriage return if ESP32 ever sends CRLF: "\r\n".
     */
    if (b == CARRIAGE_RETURN) {
        return;
    }

    /*
     * Accept only digits or N.
     */
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

    /*
     * Anything else is genuinely unknown.
     * 0xFF is already filtered above, so it will not be printed.
     */
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

    reset_reading_buffer();
    last_printed_reading[0] = '\0';

    *speaker     = 0x00;
    *green_LED   = 0x00;
    *OutPort_LED = 0x00;

    spi_flush_rx();

    /*
     * Start first SPI byte request.
     */
    spi_start_one_byte_transfer(0xFF);
}

/* ---------------- Main ---------------- */
int main(void)
{
    *SPI_SS      = 0x1;
    *OutPort_LED = 0x00;
    *green_LED   = 0x00;
    *speaker     = 0x00;

    spi_flush_rx();
    spi_interrupt_init();

    printf("============================\n");
    printf("LIVE SPI WATER METER READER\n");
    printf("FPGA = SPI master\n");
    printf("ESP32 = SPI slave\n");
    printf("ESP32 sends ASCII frame:\n");
    printf("Example: 02012\\n\n");
    printf("No detection: N\\n\n");
    printf("Frame end byte is newline: 0x0A\n");
    printf("Idle byte 0xFF is ignored\n");
    printf("============================\n");

    spi_start_water_meter_reading();

    while (1) {

        /*
         * Process completed SPI byte outside ISR.
         */
        if (spi_byte_ready) {
            spi_byte_ready = 0;
            process_received_byte();
        }

        /*
         * Request next byte from ESP32.
         */
        if (spi_running && spi_next_byte_needed) {

            spi_next_byte_needed = 0;

            /*
             * Small delay so ESP32 has time to queue the next byte.
             */
            short_delay(ESP32_QUEUE_DELAY);

            spi_start_one_byte_transfer(0xFF);
        }

        short_delay(MAIN_LOOP_DELAY);
    }

    return 0;
}
