#include "nios2_ctrl_reg_macros.h"
#include "sys/alt_stdio.h"
#include "altera_up_avalon_accelerometer_spi.h"

#include <stddef.h>
#include <math.h>

#include "system.h"
#include "hex.h"
#include "accelerometer.h"


int main(void);
void interrupt_handler(void);
void the_exception(void);

volatile int *OutPort_LEDR      = (int *)PIO_0_BASE;      // 0x4009090
volatile int *OutPort_LED8      = (int *)PIO_1_BASE;      // 0x4009080
volatile int *InPort_COUNTER    = (int *)PIO_2_BASE;      // 0x4009070
volatile int *InPort_KEY1       = (int *)PIO_3_BASE;      // 0x4009060
volatile int *GPIO2_Port        = (int *)PIO_4_BASE;      // 0x4009050
volatile int *OutPort_HEX345    = (int *)PIO_5_BASE;      // 0x4009040
volatile int *OutPort_HEX012    = (int *)PIO_6_BASE;      // 0x4009030
volatile int *InPort_SW         = (int *)PIO_7_BASE;      // 0x4009020

volatile int *UART              = (int *)UART_0_BASE;         // 0x4009000
volatile int *UART_txdata       = (int *)(UART_0_BASE + 0x4);
volatile int *UART_status       = (int *)(UART_0_BASE + 0x8);
volatile int *UART_control      = (int *)(UART_0_BASE + 0xC);

/* pio_2 is the edge-triggered interrupt input */
volatile int *Counter_IRQMask   = (int *)(PIO_2_BASE + 0x8);
volatile int *Counter_EdgeCap   = (int *)(PIO_2_BASE + 0xC);

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

/*Accelerometer INIT*/
alt_up_accelerometer_spi_dev *accel;
alt_32 x_axis;
alt_32 y_axis;
alt_32 z_axis;

int main(void)
{
	alt_putstr("HELLo\n");
	accel = alt_up_accelerometer_spi_open_dev(ACCELEROMETER_SPI_0_NAME);
    if (accel == NULL)
    {
        alt_putstr("ACCEL OPEN FAIL\n");
        while (1);
    }
    alt_putstr("ACCEL READY\n");

    *Counter_EdgeCap = 0;
    *Counter_IRQMask = 0;
    *UART_control = 0x80;
    NIOS2_WRITE_IENABLE(0x0);
    alt_putstr("IRQ BOOT\n");
    NIOS2_WRITE_STATUS(0x1);

    int prev_counter, prev_key, LED8_state = 0;
    prev_counter = (*InPort_COUNTER) & 0x01;
    prev_key = (*InPort_KEY1) & 0x01;
    *OutPort_LED8 = 1;


    for (int i = 0; i < 6; i++) hex_buf[i] = 0xFF;
    hex_refresh();

    while (1)
    {
        int mode = (*InPort_SW) & 0x3;

        if (mode == 1)
        {
        	accelerometer_main();
        }
        else if (mode == 2)
            {
        	/* ===== RX POLLING ===== */
        	    if ((*UART_status) & 0x80)
        	    {
        	        char c = (*UART) & 0xFF;

        	        if (c == '\r')
        	        {
        	            // ignore
        	        }
        	        else if (c == '\n' || rx_index >= 31)
        	        {
        	            rx_buffer[rx_index] = '\0';
        	            rx_index = 0;
        	            rx_ready = 1;
        	        }
        	        else
        	        {
        	            rx_buffer[rx_index++] = c;
        	        }
        	    }

        	    // ===== PRINT RECEIVED =====
        	    if (rx_ready)
        	    {
        	        rx_ready = 0;
        	        alt_putstr("ESP SAYS: ");
        	        alt_putstr(rx_buffer);
        	        alt_putstr("\n");
        	        hex_scroll_string((char *)rx_buffer);
        	    }

        	    // ===== KEY PRESS DETECT =====
        	    int curr_key = (*InPort_KEY1) & 0x1;

        	    if ((prev_key == 1) && (curr_key == 0))  // falling edge
        	    {
        	        alt_putstr("SENDING...\n");

        	        uart_send_string("CALL ME DADDY");
        	        uart_send_char('\n');
        	    }

        	    prev_key = curr_key;
        	}
        else {
            for (int i = 0; i < 6; i++) hex_buf[i] = 0xFF;
            hex_refresh();
            }
    }
}
