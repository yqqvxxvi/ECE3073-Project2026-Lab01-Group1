#include "system.h"
#include "unistd.h"
#include "io.h"

/* Module headers */
#include "hex.h"
#include "sw.h"
#include "uart_phrases.h"
#include "utils.h"
#include "interrupt_handler.h"
#include "nios2_ctrl_reg_macros.h"

/*============= BASE ADDRESSES =============*/
volatile int *LEDS         = (int *)PIO_0_BASE;
volatile int *HEX0         = (int *)PIO_1_BASE;
volatile int *HEX1         = (int *)PIO_2_BASE;
volatile int *HEX2         = (int *)PIO_3_BASE;
volatile int *HEX3         = (int *)PIO_4_BASE;
volatile int *HEX4         = (int *)PIO_5_BASE;
volatile int *HEX5         = (int *)PIO_6_BASE;
volatile int *SW           = (int *)PIO_7_BASE;
volatile int *KEYS         = (int *)PIO_8_BASE;
volatile int *TIMER        = (int *)TIMER_0_BASE;
volatile int *UART_control = (int *)(UART_0_BASE + 0x0C); /* altera_avalon_uart control reg (IRRDY=bit7) */


int main()
{
    /* Blank all HEX displays at startup (active-low: 0xFF = all segments off).
     * If displays go blank after reset, code IS reaching main(). */
    hex_write_all(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

    /* Enable all peripheral IRQs (UART irrdy, keys, switches) */
    irq_init();

    /* Initialise accelerometer SPI config and tap interrupt registers */
//    accelerometer_init();

    

    int prev_mode = -1;

    while (1)
    {
        /* --- Read mode first so every check below can react to a switch change --- */
        int mode = get_mode();
        const char *mode_message = NULL;

        /* --- Debounce: promote key_pending to key_flag on release --- */
        key_process_debounce();

        /* --- Handle key press (IRQ-driven, bit 0 = KEY0, bit 1 = KEY1) --- */
        if (key_flag)
        {
            int keys = key_flag;
            key_flag = 0;

            if (keys & 0x1)
            	uart_puts("KEY0 pressed\n");

            if (keys & 0x2)
            	uart_puts("KEY1 pressed\n");
        }

        /* --- Handle slide switch change (IRQ-driven) --- */
        if (sw_changed)
            sw_changed = 0;

        /* --- Mode exit cleanup --- */
        if (prev_mode == 1 && mode != 1)
            user_phrases_reset();

        switch (mode)
        {
            case 0:
                mode_message = "IDLE mode";
                break;
            case 1:
                mode_message = "Manually defined input";
                user_phrases_mode();
                break;
            case 2:
                mode_message = "Accelerometer mode";

                break;
            case 3:
                mode_message = "Mode 3";
                break;
            case 4:
                mode_message = "Image capture mode";
                break;
            case 5:
                mode_message = "CPU performance testing mode";
                break;
            default:
                uart_puts("Invalid mode\r\n");
                continue;
        }

        /* Show current mode on HEX0 (visual SW sanity check) */
        hex_show_digit(mode);

        if (mode != prev_mode)
        {
            uart_puts("Mode: ");
            uart_puts(mode_message);
            uart_puts("\r\n");
            prev_mode = mode;
        }
    }
}
