#include "nios2_ctrl_reg_macros.h"
#include "sys/alt_stdio.h"
#include "altera_up_avalon_accelerometer_spi.h"

#include <stddef.h>
#include <math.h>

#include "system.h"
#include "hex.h"
#include "accelerometer.h"
#include "sdram_test.h"

#include <stdio.h>
//#include <includes.h>
#include <stdint.h>
#include <string.h>
#include <system.h>
#include <io.h>
//
//volatile int * OutPort_LEDR = (int *) 0x08041140; // 0-7 Led
//volatile int * InPort_COUNTER = (int *) 0x08041120; // MSB
//volatile int * Key1 = (int *) 0x08041110; //
//volatile int * Key0 = (int *) 0x080410D0; //
//volatile int * SW = (int *) 0x080410C0; //
//
//volatile int * Buzzer = (int *) 0x080410B0; //
//volatile int * GreenLight = (int *) 0x080410A0; //
//volatile int * YellowLight = (int *) 0x08041090; //
//volatile int * RedLight = (int *) 0x08041080; //
//
//
//volatile int * GPIO2 = (int *) 0x08041130; //
//volatile int * LED8 = (int *) 0x08041100; //
//volatile int * UART = (int *) 0x08041040; //
//volatile int * HEX012 = (int *) 0x080410F0; //
//volatile int * HEX345 = (int *) 0x08041070; //
//
//volatile int * DRAM = (int *) 0x04000000; //
//volatile int * SPI_RxData = (int *) 0x08041000; //
//volatile int * SPI_TxData = (int *) 0x08041000; //
//volatile int * SPI_Status = (int *) 0x08041000; //
//volatile int * SPI_Control = (int *) 0x08041000; //
//volatile int * SPI_SS = (int *) 0x080410E0; //
//
//#define TRDY 0x40
//#define RRDY 0x80


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
volatile int *OutPort_HEX345    = (int *)PIO_5_BASE;      // 0x08041070
volatile int *OutPort_HEX012    = (int *)PIO_6_BASE;      // 0x080410F0
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
#define TRDY 0x40
#define RRDY 0x80

//#define TASK1_STACKSIZE 1024
//#define TASK1_PRIORITY  5

//OS_STK task1_stk[TASK1_STACKSIZE];
//typedef unsigned int OS_STK;
//#define TASK1_STACKSIZE 1024
//#define TASK1_PRIORITY  5

volatile char debug_char = 0;
volatile int debug_ready = 0;
volatile int debug_raw = 0;

int pitch_deg;
int roll_deg;
int beepOn=0;
const char *ori;


unsigned char hex_buf[6] = {0};
const char tx_message[] = "HELLO I AM GAY";

void spi_delay(void)
{
    for (volatile int i = 0; i < 1000; i++);
}

void tone_delay(volatile int count)
{
    for (volatile int i = 0; i < count; i++);
}

void beep_cycles(int cycles, int delay_count)
{
    for (int i = 0; i < cycles; i++) {
        *speaker = 0x01;          // ON
        tone_delay(delay_count);
        *speaker = 0x00;          // OFF
        tone_delay(delay_count);
    }
}

uint8_t spi_transfer(uint8_t tx)
{
    uint8_t rx;

    *SPI_SS = 0x0;
    spi_delay();

    while (!(*SPI_Status & TRDY));
    *SPI_TxData = tx;

    while (!(*SPI_Status & RRDY));
    rx = (uint8_t)(*SPI_RxData);

    spi_delay();
    *SPI_SS = 0x1;

    return rx;
}

void uart_delay(void)
{
    volatile int i;
    for (i = 0; i < 500; i++);
}

void uart_send_char(char c)
{
    *UART_txdata = (int)c;
}

void uart_send_string(const char *str)
{
    int i;
    for (i = 0; str[i] != '\0'; i++)
    {
        uart_send_char(str[i]);
        uart_delay();
    }
}

int append_byte_to_message(char *message, int *index, int max_len, char byte, char terminator)
{
	if (byte == terminator)
	{
		message[*index] = '\0';
		*index = 0;
		return 1;
	}

	if (*index < (max_len - 1))
	{
		message[(*index)++] = byte;
		return 0;
	}

	message[max_len - 1] = '\0';
	*index = 0;
	return -1;
}

void uart_byte_feedback(void)
{
	*yellow_LED = 0x01;
	beep_cycles(10, 120);
	*yellow_LED = 0x00;
}

void uart_message_feedback(void)
{
	*red_LED = 0x01;
	*green_LED = 0x01;
	beep_cycles(20, 150);
	*red_LED = 0x00;
	*green_LED = 0x00;
	*speaker = 0x00;
}

/*Accelerometer INIT*/
alt_up_accelerometer_spi_dev *accel;
alt_32 x_axis;
alt_32 y_axis;
alt_32 z_axis;

//void OSTaskCreateExt(void (*task)(void *p_arg), void *p_arg, void *p_tos,
//                     int prio, int id, void *p_bos, int stk_size,
//                     void *p_ext, int opt) {
//    task(p_arg);
//}

//volatile int sw1 = 0;
//volatile int sw2 = 0;
//volatile int sw3 = 0;
//volatile int sw4 = 0;

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

	int prev_key1, prev_key0;
    prev_key1 = (*InPort_KEY1) & 0x01;
    prev_key0 = (*InPort_KEY0) & 0x01;
    *OutPort_LED8 = 1;

    for (int i = 0; i < 6; i++) hex_buf[i] = 0xFF;
    hex_refresh();

	char uart_message[64];
	int uart_index = 0;
	char spi_message[64];
	int spi_index = 0;
	uint8_t rx;

//	(void)pdata;

	*SPI_SS = 0x1;
	*red_LED = 0x00;
	*green_LED = 0x00;   // OFF initially (active-high)
	*speaker   = 0x00;   // OFF initially

	int prev_mode = -1;
	int sdram_test_done = 0;

    while (1)
    {
    	int curr_key0 = (*InPort_KEY0) & 0x1;
		int curr_key1 = (*InPort_KEY1) & 0x1;
		int sw_raw = (*InPort_SW) & 0xF;
		int sw1 = sw_raw & 0x1;
		int sw2 = sw_raw & 0x2;
		int sw3 = sw_raw & 0x4;
		int mode = 0;

		if (sw1 == 1)
        {
			mode = 1;
		}
		else if (sw2 == 2)
        {
			mode = 2;
		}
		else if (sw3 == 4)
        {
			mode = 3;
		}

		if (mode != prev_mode)
		{
			if (mode == 1)
			{
				alt_putstr("SLIDE SWITCH 1 ON\n");
			}
			else if (mode == 2)
			{
				alt_putstr("SLIDE SWITCH 2 ON\n");
			}
			else if (mode == 3)
			{
				alt_putstr("SLIDE SWITCH 3 ON (SDRAM TEST)\n");
			}
			else
			{
				alt_putstr("SLIDE SWITCH MODE OFF\n");
			}
			prev_mode = mode;
		}


        if (mode == 1)
        {
        	accelerometer_main();
        	
        }

        else if (mode == 2)
            {
				if ((prev_key0 == 1) && (curr_key0 == 0))  // falling edge: KEY0 -> UART
				{
					if ((*UART_status) & 0x80)
					{
						char c = (*UART) & 0xFF;

						if (c != '\r')
						{
							int uart_result = append_byte_to_message(uart_message,
							                                         &uart_index,
							                                         (int)sizeof(uart_message),
							                                         c,
							                                         '\n');

							uart_byte_feedback();

							if (uart_result == 1)
							{
								alt_putstr("UART sentence: ");
								alt_putstr(uart_message);
								alt_putstr("\n");
								uart_message_feedback();
								hex_scroll_string(uart_message);
							}
							else if (uart_result == -1)
							{
								alt_putstr("UART buffer overflow\n");
								*speaker = 0x00;
								*yellow_LED = 0x00;
							}
						}
					}
					else
					{
						alt_putstr("UART no data ready\n");
					}
				}

				if ((prev_key1 == 1) && (curr_key1 == 0))  // falling edge: KEY1 -> SPI
				{
					rx = spi_transfer(0xFF);

					int spi_result = append_byte_to_message(spi_message,
					                                     &spi_index,
					                                     (int)sizeof(spi_message),
					                                     (char)rx,
					                                     '\0');

					uart_byte_feedback();

					if (spi_result == 1)
					{
						alt_putstr("SPI sentence: ");
						alt_putstr(spi_message);
						alt_putstr("\n");
						uart_message_feedback();
						hex_scroll_string(spi_message);
					}
					else if (spi_result == -1)
					{
						alt_putstr("SPI buffer overflow\n");
					}
				}
        	}
	        else if (mode == 3)
	        {
				if (!sdram_test_done)
				{
					sdram_run_default_test();
					sdram_test_done = 1;
				}
			}
        else {
            for (int i = 0; i < 6; i++) hex_buf[i] = 0xFF;
            hex_refresh();
			*speaker = 0x00;
			*red_LED = 0x00;
			*green_LED = 0x00;
			*yellow_LED = 0x00;
            }

			if (mode != 3)
			{
				sdram_test_done = 0;
			}

		prev_key0 = curr_key0;
		prev_key1 = curr_key1;
		for (volatile int i = 0; i < 1000; i++);
    }
}

//
//int main(void)
//{
//    OSInit();
//
//    printf("============================\n");
//
//    OSTaskCreateExt(task1,
//                    NULL,
//                    (void *)&task1_stk[TASK1_STACKSIZE - 1],
//                    TASK1_PRIORITY,
//                    TASK1_PRIORITY,
//                    task1_stk,
//                    TASK1_STACKSIZE,
//                    NULL,
//                    0);
//
//    OSStart();
//    return 0;
//}

