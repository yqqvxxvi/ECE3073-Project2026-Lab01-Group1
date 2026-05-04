#include "nios2_ctrl_reg_macros.h"
#include "altera_up_avalon_accelerometer_spi.h"
#include "system.h"
#include "hex.h"
#include "accelerometer.h"

#include <stdlib.h>
#include "altera_avalon_mutex.h"
#include <sys/alt_cache.h>


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <system.h>
#include <io.h>
#include <sys/alt_irq.h>
#include "sys/alt_irq.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"


#include "sdram.h"

///* ---------------- SPI registers ---------------- */
//volatile int *SPI_RxData  = (int *)0x08009000;
//volatile int *SPI_TxData  = (int *)0x08009004;
//volatile int *SPI_Status  = (int *)0x08009008;
//volatile int *SPI_Control = (int *)0x0800900C;
//
///* ---------------- PIO registers ---------------- */
//volatile int *SPI_SS      = (int *)0x080090c0;
//volatile int *OutPort_LED = (int *)0x080090e0;
//volatile int *green_LED   = (int *)0x080090b0;
//volatile int *speaker     = (int *)0x080090a0;
//
//volatile int *KEY0        = (int *)0x08009090;
//volatile int *KEY1        = (int *)0x080090f0;

//#define HEX012_BASE 				0x00021030
//#define HEX345_BASE 				0x00021020
//#define KEY01_BASE 					0x00021080
//#define SDRAM_MUTEX_BASE 			0x02021080
//#define SHARED_SDRAM_BASE 			0x01000000
#define SDRAM_1       				0x01100000
#define SDRAM_2       				0x01200000
//#define ACCELEROMETER_SPI_0_BASE 	0x00021098
//#define SPI_0_BASE 					0x00021000
//#define SPI_SS_BASE 				0x00021050
//#define US_COUNTER_BASE 			0x02021070


volatile int *timer      = (int *)US_COUNTER_BASE;
volatile int *OutPort_HEX012      = (int *)HEX012_BASE;
volatile int *OutPort_HEX345      = (int *)HEX345_BASE;
volatile int *ACCEL      = (int *)ACCELEROMETER_SPI_0_BASE;
volatile int *SPI_RxData  = (int *) SPI_0_BASE;
volatile int *SPI_TxData  = (int *) (SPI_0_BASE + 0x4);
volatile int *SPI_Status  = (int *) (SPI_0_BASE + 0x8);
volatile int *SPI_Control = (int *) (SPI_0_BASE + 0xC);
volatile int *SPI_SS      = (int *)SPI_SS_BASE;

#define TRDY 0x40
#define RRDY 0x80

volatile int *Keys       = (int *)KEY01_BASE;

volatile int *Mutex      = (int *)SDRAM_MUTEX_BASE;
volatile int *Shared_Sdram_Flag      = (int *)SHARED_SDRAM_BASE;
volatile int *Shared_Sdram_1      = (int *)SDRAM_1;
volatile int *Shared_Sdram_2      = (int *)SDRAM_2;

/* pio_2 is the edge-triggered interrupt input */
//volatile int *Counter_IRQMask   = (int *)(PIO_2_BASE + 0x8);
//volatile int *Counter_EdgeCap   = (int *)(PIO_2_BASE + 0xC);

volatile char rx_buffer[32];
volatile int rx_index = 0;
volatile int rx_ready = 0;


volatile char debug_char = 0;
volatile int debug_ready = 0;
volatile int debug_raw = 0;

int pitch_deg;
int roll_deg;
const char *ori;


unsigned char hex_buf[6] = {0};
const char tx_message[] = "HELLO I AM GAY";

/*Accelerometer INIT*/
alt_up_accelerometer_spi_dev *accel;
alt_32 x_axis;
alt_32 y_axis;
alt_32 z_axis;
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
#define RGB30(r, g, b) (((r & 0x3FF) << 20) | ((g & 0x3FF) << 10) | (b & 0x3FF))

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
         * Start storing after this.
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
         *
         * This fixes the problem where the first few reads may begin
         * in the middle of the ESP32 sequence.
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
         * Stop storing here.
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
             * We do not start it directly here because ESP32 needs time
             * to queue its next byte.
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
    alt_irq_register(
        SPI_0_IRQ_INTERRUPT_CONTROLLER_ID,
        SPI_0_IRQ,
        spi_isr
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

//    *speaker     = 0x00;
//    *green_LED   = 0x00;
//    *OutPort_LED = 0x00;

    spi_flush_rx();

    printf("START SPI MARKED STRING RECEIVE USING INTERRUPTS\n");
    printf("Waiting for '<' start marker...\n");

    /*
     * Start the first byte transfer.
     * When the byte arrives, spi_isr() will run.
     */
    spi_start_one_byte_transfer(0xFF);
}

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

//    *OutPort_LED = 0x01;
//    *green_LED   = 0x01;

//    beep_short();
//
//    *speaker     = 0x00;
//    *OutPort_LED = 0x00;
//    *green_LED   = 0x00;
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

// Example: Converting RGB565 (from Grove) to 30-bit RGB (for VGA)
void process_camera_data(alt_up_pixel_buffer_dma_dev* pixel_buffer) {
	uint16_t CAM_HEIGHT = 0;
	uint16_t CAM_WIDTH = 0;
	int byte=0;
	int index=0;
	sdram_read_array(index,byte,1);

	while(byte != 0x80)
	{
		sdram_read_array(index,byte,1);
		index++;
	}


	for (int y = 0; y < CAM_HEIGHT; y++) {
        for (int x = 0; x < CAM_WIDTH; x++) {
            // 1. Get 2 bytes from SPI (RGB565)
//        	uint8_t b1 = spi_read_byte();
//			uint8_t b2 = spi_read_byte();
			uint8_t b1;
			uint8_t b2;

        	sdram_read_array(index,b1,1);
			index++;
			sdram_read_array(index,b2,1);
			index++;

            uint16_t rgb565 = (b1 << 8) | b2;

            // 2. Extract 5-6-5 bits
            uint32_t r5 = (rgb565 >> 11) & 0x1F;
            uint32_t g6 = (rgb565 >> 5) & 0x3F;
            uint32_t b5 = (rgb565 & 0x1F);

            // 3. Upscale to 10-bit per channel (30-bit total)
            uint32_t r10 = (r5 << 5) | (r5 >> 0); // Simple bit-padding
            uint32_t g10 = (g6 << 4) | (g6 >> 2);
            uint32_t b10 = (b5 << 5) | (b5 >> 0);

            uint32_t color30 = (r10 << 20) | (g10 << 10) | b10;

            // 4. Write to the Pixel Buffer
            alt_up_pixel_buffer_dma_draw(pixel_buffer, color30, x, y);
        }
    }
}


int main()
{ 
	uint32_t start_time, end_time;
	start_time = IORD(timer, 0);
	alt_putstr("Hello from core 2!\n");
	for (int i = 0; i < 200000; i++){i++;};
	end_time = IORD(timer, 0);

	printf("Time(us): %u\n\n",(unsigned int)(end_time-start_time));
	alt_mutex_dev* mutex=altera_avalon_mutex_open("/dev/SHARED_SDRAM");

	//  int LENGTH;
	alt_dcache_flush_all();
	while((IORD_16DIRECT(Shared_Sdram_Flag,0x10))==0x0);

	alt_up_pixel_buffer_dma_dev* pixel_buffer = alt_up_pixel_buffer_dma_open_dev("/dev/video_pixel_buffer_dma_0");

	alt_up_char_buffer_dev* char_buffer = alt_up_char_buffer_open_dev("/dev/video_character_buffer_with_dma_0_avalon_char_buffer_slave");

	if (pixel_buffer == NULL) {
	  // Handle error: Device not found
	  return -1;
	}

	// Optional: Clear the screen with a specific color (e.g., black)
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 1);

	accel = alt_up_accelerometer_spi_open_dev(ACCELEROMETER_SPI_0_NAME);
	if (accel == NULL)
	{
	  alt_putstr("ACCEL OPEN FAIL\n");
	  while (1);
	}
	alt_putstr("ACCEL READY\n");

	NIOS2_WRITE_IENABLE(0x0);
	alt_putstr("IRQ BOOT\n");
	NIOS2_WRITE_STATUS(0x1);

	for (int i = 0; i < 6; i++) hex_buf[i] = 0xFF;
	hex_refresh();

	*SPI_SS      = 0x1;
	int prev_key0;
	int prev_key1;
	int curr_key0;
	int curr_key1;
	int pitch;
	int roll;
	int z;

while (1)
	{

	//draw to VGA
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 1);

	alt_up_pixel_buffer_dma_draw(pixel_buffer, RGB30(1023, 0, 0), 100, 100);
	alt_up_pixel_buffer_dma_draw_line(pixel_buffer, 50, 50, 100, 100, RGB30(0, 1023, 0), 0);
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 150, 150, 200, 200, RGB30(0, 0, 1023), 0);
	alt_up_char_buffer_draw(char_buffer, (unsigned char)'A', 0, 0);
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 1);

//	x += (direction * 5);
//	if (x > 580 || x < 0) direction *= -1;
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
//	while (alt_up_pixel_buffer_dma_check_storage_sync(pixel_buffer));

	altera_avalon_mutex_lock(mutex, 0);
	unsigned int swBase = IORD_16DIRECT(Shared_Sdram_Flag,0x10);
//	unsigned int keyBase = IORD_16DIRECT(Shared_Sdram_Flag,0x20);
    altera_avalon_mutex_unlock(mutex);
    int keyBase= IORD_32DIRECT(Keys, 0);

    unsigned int sw1 = swBase & 0x1;
    curr_key0 = keyBase & 0x1;
    curr_key1 = keyBase & 0x2;
	printf("%u",sw1);
	if (sw1 == 1)
	{
		accelerometer_main(&pitch,&roll,&z);
	}

	else
	{
//	    printf("%u",sw1);
		for (int i = 0; i < 6; i++) hex_buf[i] = 0xFF;
		hex_refresh();
		*OutPort_HEX345 =
			((int)hex_buf[2] << 16) |
			((int)hex_buf[1] << 8)  |
			((int)hex_buf[0]);

		*OutPort_HEX012 =
			((int)hex_buf[5] << 16) |
			((int)hex_buf[4] << 8)  |
			((int)hex_buf[3]);;
	}

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
	 * Do SDRAM write and printf outside the ISR.
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

//            *speaker   = 0x00;
//            *green_LED = 0x00;
		*SPI_SS    = 0x1;
	}

	/*
	 * KEY1 active-low press.
	 */
	if ((prev_key1 == 1) && (curr_key1 == 0)) {

		short_delay(KEY_DEBOUNCE_DELAY);

		printf("\nKEY1 PRESSED\n");

		print_sentence_from_sdram();
		process_camera_data(pixel_buffer);


	}

	prev_key0 = curr_key0;
	prev_key1 = curr_key1;

	short_delay(MAIN_LOOP_DELAY);
    }
}

