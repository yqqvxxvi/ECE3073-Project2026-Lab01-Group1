#include "system.h"
#include "unistd.h"
#include "io.h"
#include <stdio.h>

/* Module headers */
#include "hex.h"
#include "sw.h"
#include "uart_phrases.h"
#include "utils.h"
#include "interrupt_handler.h"
#include "accelerometer.h"


/*============= FALL BACK BASE ADDRESSES =============*/
#ifndef PIO_7_BASE
#define PIO_7_BASE 0x4001060
#endif

#ifndef PIO_8_BASE
#define PIO_8_BASE 0x4001050
#endif

#ifndef TIMER_0_BASE
#define TIMER_0_BASE 0x4001000
#endif

#ifndef SPI_0_BASE
#define SPI_0_BASE 0x4001020
#endif


/*============= BASE ADDRESSES =============*/
volatile int *LEDS  = (int *)PIO_0_BASE;
volatile int *HEX0  = (int *)PIO_1_BASE;
volatile int *HEX1  = (int *)PIO_2_BASE;
volatile int *HEX2  = (int *)PIO_3_BASE;
volatile int *HEX3  = (int *)PIO_4_BASE;
volatile int *HEX4  = (int *)PIO_5_BASE;
volatile int *HEX5  = (int *)PIO_6_BASE;
volatile int *SW    = (int *)PIO_7_BASE;
volatile int *KEYS  = (int *)PIO_8_BASE;
volatile int *TIMER = (int *)TIMER_0_BASE;
volatile int *SPI   = (int *)SPI_0_BASE;


int main()
{
    /* Initialise accelerometer SPI config and tap interrupt registers */
    accelerometer_init();

    /* Enable timer, key, switch, and gsensor IRQs on the NIOS2 */
    irq_init();

    int prev_mode = -1;

    while (1)
    {
        /* --- Handle key press (IRQ-driven, bit 0 = KEY0, bit 1 = KEY1) --- */
        if (key_flag)
        {
            int keys  = key_flag;
            key_flag  = 0;

            if (keys & 0x1)
                printf("KEY0 pressed\n");

            if (keys & 0x2)
                printf("KEY1 pressed\n");
        }

        /* --- Handle slide switch change (IRQ-driven) --- */
        if (sw_changed)
        {
            sw_changed = 0;
            /* get_mode() reads current SW value directly */
        }

        /* --- Handle accelerometer tap (IRQ-driven) --- */
        if (gsensor_flag)
        {
            gsensor_flag = 0;
            uint8_t src = accelerometer_clear_interrupt(); /* clears ADXL345 IRQ */

            if (src & ADXL345_INT_DOUBLE_TAP)
                printf("Double tap detected\n");
            else if (src & ADXL345_INT_SINGLE_TAP)
                printf("Single tap detected\n");
        }

        /* --- Mode switch --- */
        int mode = get_mode();
        const char *mode_message = NULL;

        if (prev_mode == 1 && mode != 1)
        {
            user_phrases_reset();
        }

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
                printf("Invalid mode: %d\n", mode);
                continue;
        }

        if (mode != prev_mode)
        {
            printf("Current mode: %s\n", mode_message);
            prev_mode = mode;
        }
    }
}
