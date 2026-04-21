#include "nios2_ctrl_reg_macros.h"
#include "sys/alt_stdio.h"
#include "altera_up_avalon_accelerometer_spi.h"

#include <stddef.h>
#include <math.h>

#include "system.h"
#include "hex.h"
#include "accelerometer.h"
#include "vga.h"
#include <stdio.h>
//#include <includes.h> FOR RTOS, NOT NEEDED HERE
#include <stdint.h>
#include <string.h>
#include <system.h>
#include <io.h>

int main(void);
void interrupt_handler(void);
void the_exception(void);

#define PIO_0_BASE 0x08041140
#define PIO_1_BASE 0x08041100

#define PIO_2_BASE 0x08041120

#define PIO_3_BASE 0x08041110
#define PIO_4_BASE 0x08041130
#define PIO_5_BASE 0x08041070
#define PIO_6_BASE 0x080410F0
#define PIO_7_BASE 0x080410C0


volatile int *OutPort_LEDR      = (int *)PIO_0_BASE;      // 0x08041140
volatile int *OutPort_LED8      = (int *)PIO_1_BASE;      // 0x08041100
volatile int *InPort_COUNTER    = (int *)PIO_2_BASE;      // 0x08041120 MSB
volatile int *InPort_KEY0       = (int *)0x080410D0;      // 0x08041110
volatile int *InPort_KEY1       = (int *)PIO_3_BASE;      // 0x08041110

volatile int *GPIO2_Port        = (int *)PIO_4_BASE;      // 0x08041130
volatile int *OutPort_HEX345    = (int *)0x08041070;      // 0x08041070
volatile int *OutPort_HEX012    = (int *)0x080410F0;      // 0x080410F0
volatile int *InPort_SW         = (int *)PIO_7_BASE;      // 0x080410C0

#define UART_0_BASE 0x08041040
volatile int *UART              = (int *)UART_0_BASE;         // 0x4009000
volatile int *UART_txdata       = (int *)(UART_0_BASE + 0x4);
volatile int *UART_status       = (int *)(UART_0_BASE + 0x8);
volatile int *UART_control      = (int *)(UART_0_BASE + 0xC);

/* pio_2 is the edge-triggered interrupt input */
volatile int *Counter_IRQMask   = (int *)(PIO_2_BASE + 0x8);
volatile int *Counter_EdgeCap   = (int *)(PIO_2_BASE + 0xC);

volatile int * speaker     = (int *) 0x080410B0;
volatile int * green_LED = (int *) 0x080410A0; //
volatile int * yellow_LED = (int *) 0x08041090; //
volatile int * red_LED = (int *) 0x08041080; //

#define SPI_0_BASE 0x08041000
volatile int * SPI_RxData  = (int *) SPI_0_BASE;
volatile int * SPI_TxData  = (int *) (SPI_0_BASE + 0x4);
volatile int * SPI_Status  = (int *) (SPI_0_BASE + 0x8);
volatile int * SPI_Control = (int *) (SPI_0_BASE + 0xC);

volatile int * SPI_SS      = (int *) 0x080410E0;

volatile int * SDRAM = (int *) (0x04000000);

#define TRDY 0x40
#define RRDY 0x80

// FOR RTOS, NOT NEEDED HERE
//#define TASK1_STACKSIZE 1024
//#define TASK1_PRIORITY  5

//OS_STK task1_stk[TASK1_STACKSIZE];
//typedef unsigned int OS_STK;
//#define TASK1_STACKSIZE 1024
//#define TASK1_PRIORITY  5

volatile char rx_buffer[32];
volatile int rx_index = 0;
volatile int rx_ready = 0;


volatile char debug_char = 0;
volatile int debug_ready = 0;
volatile int debug_raw = 0;

int pitch_deg;
int roll_deg;
int beepOn=0;
const char *ori;


unsigned char hex_buf[6] = {0};
const char tx_message[] = "HELLO";


void dance(int bpm)
{
    if (bpm == 0) bpm = 120;

    // beat period in delay counts (tune multiplier to your clock)
    int beat = 60000000 / bpm;
    int half = beat / 2;
    int qtr  = beat / 4;

    // --- bar 1: four beats, all LEDs cycle ---
    int b;
    for (b = 0; b < 4; b++)
    {
        // beat ON
        IOWR(red_LED,    0, 0x1);
        IOWR(green_LED,  0, 0x0);
        IOWR(yellow_LED, 0, 0x0);
        IOWR(speaker,    0, 0x1);
        for (volatile int i = 0; i < half; i++);

        // beat OFF
        IOWR(red_LED,    0, 0x0);
        IOWR(green_LED,  0, 0x1);
        IOWR(yellow_LED, 0, 0x0);
        IOWR(speaker,    0, 0x0);
        for (volatile int i = 0; i < half; i++);
    }

    // --- bar 2: fast alternating yellow/green on off-beats ---
    for (b = 0; b < 8; b++)
    {
        IOWR(yellow_LED, 0, (b % 2 == 0) ? 0x1 : 0x0);
        IOWR(green_LED,  0, (b % 2 == 0) ? 0x0 : 0x1);
        IOWR(red_LED,    0, 0x0);
        IOWR(speaker,    0, 0x1);
        for (volatile int i = 0; i < qtr; i++);
        IOWR(speaker,    0, 0x0);
        for (volatile int i = 0; i < qtr; i++);
    }

    // --- bar 3: all flash together x4 ---
    for (b = 0; b < 4; b++)
    {
        IOWR(red_LED,    0, 0x1);
        IOWR(green_LED,  0, 0x1);
        IOWR(yellow_LED, 0, 0x1);
        IOWR(speaker,    0, 0x1);
        for (volatile int i = 0; i < qtr; i++);

        IOWR(red_LED,    0, 0x0);
        IOWR(green_LED,  0, 0x0);
        IOWR(yellow_LED, 0, 0x0);
        IOWR(speaker,    0, 0x0);
        for (volatile int i = 0; i < qtr; i++);
    }

    // --- bar 4: chase pattern red -> yellow -> green ---
    int chase;
    for (chase = 0; chase < 3; chase++)
    {
        IOWR(red_LED,    0, 0x1);
        IOWR(yellow_LED, 0, 0x0);
        IOWR(green_LED,  0, 0x0);
        IOWR(speaker,    0, 0x1);
        for (volatile int i = 0; i < half; i++);
        IOWR(speaker,    0, 0x0);

        IOWR(red_LED,    0, 0x0);
        IOWR(yellow_LED, 0, 0x1);
        IOWR(green_LED,  0, 0x0);
        IOWR(speaker,    0, 0x1);
        for (volatile int i = 0; i < half; i++);
        IOWR(speaker,    0, 0x0);

        IOWR(red_LED,    0, 0x0);
        IOWR(yellow_LED, 0, 0x0);
        IOWR(green_LED,  0, 0x1);
        IOWR(speaker,    0, 0x1);
        for (volatile int i = 0; i < half; i++);
        IOWR(speaker,    0, 0x0);
    }

    // --- finale: all off ---
    IOWR(red_LED,    0, 0x0);
    IOWR(green_LED,  0, 0x0);
    IOWR(yellow_LED, 0, 0x0);
    IOWR(speaker,    0, 0x0);
}

void tone_delay(volatile int count)
{
    for (volatile int i = 0; i < count; i++);
}

void beep_cycles(int cycles, int delay_count)
{
    if (delay_count==0x00){
    	delay_count=0x01;
    }

	for (int i = 0; i < cycles; i++) {
        IOWR(speaker, 0, 1);         // ON
        tone_delay(delay_count);
        IOWR(speaker, 0, 0);          // OFF
        tone_delay(delay_count);
    }
}

void spi_delay(void)
{
    for (volatile int i = 0; i < 1000; i++);
}

uint8_t spi_transfer(uint8_t tx)
{
    uint8_t rx;

    *SPI_SS = 0x0;
    spi_delay();

    while (!(IORD_32DIRECT(SPI_Status, 0) & TRDY));
//    *SPI_TxData = tx;
    IOWR_32DIRECT(SPI_TxData,0,tx);

    while (!(IORD_32DIRECT(SPI_Status, 0) & RRDY));
//    rx = (uint8_t)(*SPI_RxData);
    rx = (uint8_t)IORD_32DIRECT(SPI_RxData, 0);

    spi_delay();
//    *SPI_SS = 0x1;
    IOWR_32DIRECT(SPI_SS,0,0x1);

    return rx;
}

void uart_delay(void)
{
    volatile int i;
    for (i = 0; i < 10000; i++);
}

void uart_send_char(char c)
{
//    *UART_txdata = (int)c;
    IOWR_32DIRECT(UART_txdata,0,(int)c);
}

void uart_send_string(const char *str)
{
    int i=0;
    for (i = 0; str[i] != '\0'; i++)
    {
    	while (!(IORD_32DIRECT(UART_status, 0) & 0x20)){};
//        uart_send_char(str[i]);
        IOWR_32DIRECT(UART_txdata,0,(int)str[i]);
//        uart_delay();
    }
}

void SDRAM_WriteChar(uint32_t offset, char str) {
		IOWR_16DIRECT(SDRAM,offset*4,str);
	    }
void SDRAM_ReadChar(uint32_t offset, int size) {
	if (size <= 0) return;
	// 1. Allocate size + 1 to account for the '\0'
	char *hold = (char *)malloc(size + 1);
	if (hold == NULL) return; // Always check for small heap systems

	int i;
	for (i = 0; i < size; i++) {
		// 2. Mask to ensure you only get 8-bit char data if that's what you want
		hold[i] = (char)(IORD_16DIRECT(SDRAM, offset + i * 4) & 0xFF);
//		alt_putstr("read");
	}

	hold[size] = '\0'; // 3. Safe because we allocated size + 1
	alt_putstr("DRAM READ:");
	alt_putstr(hold);
	alt_putstr("\n");

	free(hold); // 4. Don't forget to free if using malloc
}

char* SDRAM_ReadChar_no_free(uint32_t offset, int size) {
    if (size <= 0) return NULL;

    char *hold = (char *)malloc(size + 1);
    if (hold == NULL) return NULL;

    int i;
    for (i = 0; i < size; i++) {
        hold[i] = (char)(IORD_16DIRECT(SDRAM, offset + i * 4) & 0xFF);
    }
    hold[size] = '\0';

    // Print to JTAG terminal
    alt_putstr("DRAM READ:");
    alt_putstr(hold);
    alt_putstr("\n");

    // Send over UART
    uart_send_string("DRAM:");
    for (i = 0; i < size; i++) {
        while (!(IORD_32DIRECT(UART_status, 0) & 0x20));
        IOWR_32DIRECT(UART_txdata, 0, (int)hold[i]);
    }
    while (!(IORD_32DIRECT(UART_status, 0) & 0x20));
    IOWR_32DIRECT(UART_txdata, 0, (int)'\n');

    return hold;   // caller is responsible for free()
}

/*Accelerometer INIT*/
alt_up_accelerometer_spi_dev *accel;
alt_32 x_axis;
alt_32 y_axis;
alt_32 z_axis;

const char *str;
const char *strSPI;
int rx_end;

int main(void)
{

	accel = alt_up_accelerometer_spi_open_dev(ACCELEROMETER_SPI_0_NAME);
    if (accel == NULL)
    {
        alt_putstr("ACCEL OPEN FAIL\n");
        while (1);
    }
    alt_putstr("ACCEL READY\n");

    *Counter_EdgeCap = 0;
    *Counter_IRQMask = 1;
    *UART_control = 0x80;
    NIOS2_WRITE_IENABLE(0x0);
    alt_putstr("IRQ BOOT\n");
    NIOS2_WRITE_STATUS(0x1);

    int prev_counter, prev_key1,prev_key0, LED8_state = 0;
    prev_counter = (*InPort_COUNTER) & 0x01;
    prev_key1 = (*InPort_KEY1) & 0x01;
    prev_key0 = (*InPort_KEY0) & 0x01;
    *OutPort_LED8 = 1;

    for (int i = 0; i < 6; i++) hex_buf[i] = 0xFF;
    hex_refresh();

//	IOWR_32DIRECT(UART,0,0x00);
//	IOWR_32DIRECT(UART_txdata,0,0x00);
//	IOWR_32DIRECT(UART_status,0,0x00);
//	IOWR_32DIRECT(UART_control,0,0x80);

	IOWR_32DIRECT(OutPort_LEDR,0,0x00);
	int max_index=0;

    char message[512];
	int index = 0;
	uint8_t rx=20;

	*SPI_SS = 0x1;
	*red_LED = 0x00;
	*green_LED = 0x00;   // OFF initially (active-high)
	*speaker   = 0x00;   // OFF initially

	int UartSending=0;
	int i=0;

	int SPISending=0;
	int j=0;

//	while (!(IORD_32DIRECT(SPI_Status, 0) & RRDY));
	(uint8_t)IORD_32DIRECT(SPI_RxData, 0); //clear Rx

    while (1)
    {
//    	int sw1 = (*InPort_SW) & 0x1;
    	int swBase = IORD_32DIRECT(InPort_SW, 0);
    	int sw1 = swBase & 0x1;
    	int sw2 = swBase & 0x2;

    	if (IORD_32DIRECT(UART_status, 0) & 0x80)
		{
    		char c = IORD_32DIRECT(UART, 0);
			if (c == '\r')
			{
				// ignore
			}
			else if (c == '\n' || rx_index >= 31)
			{
				rx_buffer[rx_index] = '\0';
				rx_end=rx_index;
				rx_index = 0;
				rx_ready = 1;
			}
			else
			{
				rx_buffer[rx_index++] = c;
			}
		}

		// ===== PRINT RECEIVED =====
		if (rx_ready==1)
		{
			rx_ready = 0;
			alt_putstr("ESP SAYS: ");
			alt_putstr(rx_buffer);
			alt_putstr("\n" );
//			hex_scroll_string((char *)rx_buffer);
////			for (i = 0; rx_buffer[i] != '\0'; i++){
////				SDRAM_WriteChar(i, rx_buffer[i]);
////				}
//			SDRAM_ReadChar(0, max_index);
////			i=0;
//			 // ===== blink and beep on receive =====
//				beep_cycles(80, rx_index);
//			    IOWR(red_LED,  0, 0x1);
//			    IOWR(green_LED,0, 0x1);
//			    for (volatile int i = 0; i < 500000; i++);   // ~100ms visual flash
//
//				IOWR(red_LED,  0, 0x0);
//				IOWR(green_LED,0, 0x0);
			// ===== CHECK ESP32 RESPONSE =====
			    if (strcmp((char *)rx_buffer, "YES DADDY HOWGEN") == 0)
			    {
			        alt_putstr("GOT\n");
			        hex_scroll_string("READY READ SDRAM", 1);

			        // Store to SDRAM
			        int i;
			        for (i = 0; rx_buffer[i] != '\0'; i++) {
			            SDRAM_WriteChar(i, rx_buffer[i]);
			        }
			        max_index = i;

			        // Read back, send over UART, show on HEX
			        char *dram_data = SDRAM_ReadChar_no_free(0, max_index);
			        if (dram_data != NULL) {
			            hex_scroll_string(dram_data, 1);
			            free(dram_data);
			        }
			    }
			    else
			    {
			        // Unexpected response
			        alt_putstr("UNEXPECTED RESPONSE\n");
			        hex_scroll_string((char *)rx_buffer, 0);

			        red_blink(10, 10000);
			    }

		}
		if (UartSending==1){
			if (str[i] != '\0'){
				if ((IORD_32DIRECT(UART_status, 0) & 0x20)){
					IOWR_32DIRECT(UART_txdata,0,(int)str[i]); // put str on UART.
					i++;
					beep_cycles(80, i);
					IOWR(red_LED,  0, 0x1);
					IOWR(yellow_LED, 0 , 0x1);
					IOWR(green_LED,0, 0x1);


					for (volatile int k = 0; k < 50000; k++);
					IOWR(red_LED,  0, 0x0);
					IOWR(green_LED,0, 0x0);
				}
			}
			else{
				UartSending=0;
				i=0;
				uart_send_char('\n');
			};
		}


		// ===== KEY0 PRESS DETECT =====
//		int curr_key0 = (*InPort_KEY0) & 0x1;
		int curr_key0 = IORD_32DIRECT(InPort_KEY0, 0)& 0x1;
		if ((prev_key0 == 1) && (curr_key0 == 0))  // falling edge
		{
			alt_putstr("SENDING...\n");
			UartSending=1;
			str ="CALL ME DADDY";
		}

		prev_key0 = curr_key0;


		for (volatile int i = 0; i < 1000; i++);
		// ===== KEY1 PRESS DETECT =====

		int curr_key1 = IORD_32DIRECT(InPort_KEY1, 0)& 0x1;
		if ((prev_key1 == 1) && (curr_key1 == 0))  // falling edge
			{
			printf("\nSTART SPI STRING RECEIVE\n");

//			rx = spi_transfer(0x00);
			strSPI="ABCDEFGHIJKLMNOPQRSTUVWXYZ123456780";
			SPISending=1;
			}

		if (SPISending==1){
			*SPI_SS = 0x0;
			spi_delay();
			if (strSPI[j] != 0x00){
				while (!(IORD_32DIRECT(SPI_Status, 0) & TRDY));
				IOWR_32DIRECT(SPI_TxData,0,strSPI[j]);
				j++;
				}

			else{
				SPISending=0;
				while (!(IORD_32DIRECT(SPI_Status, 0) & TRDY));
				IOWR_32DIRECT(SPI_TxData,0,strSPI[j]);
				j=0;
				};
			spi_delay();
			IOWR_32DIRECT(SPI_SS,0,0x1);
//			*SPI_SS = 0x1;
		}

		//check for recive
		if ((IORD_32DIRECT(SPI_Status, 0) & RRDY)){
			rx = (uint8_t)IORD_32DIRECT(SPI_RxData, 0);

			printf("Received byte: 0x%02X\n", rx);
			beep_cycles(80, rx);
			message[index] = (char)rx;
			SDRAM_WriteChar(index, (char)rx);
			index++;
		}

		prev_key1 = curr_key1;
		for (volatile int i = 0; i < 1000; i++);

		if (rx >= 32 && rx <= 126) {
			printf(" ('%c')", rx);
			rx=20;
		}

		if (rx == '\0') {
			printf("Full sentence: %s\n", message);
			printf("ON-------------------------\n");
			hex_scroll_string((char *)message, 0);

			IOWR(speaker,0,0x1);
			IOWR(red_LED,0,0x1);
			IOWR(green_LED,0,0x1);

			volatile int i;
			for (i = 0; i < 5000000; i++); //1 sec delay

			IOWR(red_LED,0,0x0);
			IOWR(green_LED,0,0x0);

			max_index=index;
			index = 0;
			rx=20;
			printf("OFF-------------------------\n");
			}
		else {
			IOWR(green_LED,0,0x0);    // keep OFF until full sentence

		}

        if (sw1 == 1)
        {
        	accelerometer_main();
        }



        else {
        	for (int i = 0; i < 6; i++) hex_buf[i] = 0xFF;
        	hex_refresh();
        }
        if (sw2 == 2)
            {
        	dance(0);     // default 120 BPM
        	}

//            alt_putstr("switch2off\n");
    }
}
