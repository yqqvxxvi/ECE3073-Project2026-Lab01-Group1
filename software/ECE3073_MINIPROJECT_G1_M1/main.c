#include "system.h"
#include "unistd.h"
#include "io.h"
#include <stdio.h>

/* Header of different modules */
#include "hex.h"
#include "sw.h"
#include "uart_phrases.h"
#include "utils.h"

#ifndef PIO_7_BASE
#define PIO_7_BASE 0x4001010
#endif

#ifndef PIO_8_BASE
#define PIO_8_BASE 0x4001000
#endif

/*============= BASE ADDRESSES =============*/
volatile int *LEDS = (int *)PIO_0_BASE; // all this are in system.h just in case u dunno
volatile int *HEX0 = (int *)PIO_1_BASE; // calling the address like this so we don't need to refer the platform designer
volatile int *HEX1 = (int *)PIO_2_BASE; // or the system.h every time after we make some changes on the NIOS2 architecture.
volatile int *HEX2 = (int *)PIO_3_BASE;
volatile int *HEX3 = (int *)PIO_4_BASE;
volatile int *HEX4 = (int *)PIO_5_BASE;
volatile int *HEX5 = (int *)PIO_6_BASE;
volatile int *SW = (int *) PIO_7_BASE;
volatile int *KEYS = (int *) PIO_8_BASE;


int main()
{
    int prev_mode = -1;
    while (1)
    {
        int mode = get_mode();
        const char *mode_message = NULL;

        if (prev_mode == 1 && mode != 1)
        {
            user_phrases_reset();
        }

        switch (mode)
        {
            case 0: // IDLE mode, do nothing
                mode_message = "IDLE mode";
                break;
            case 1: // User input mode, input 2 phrases on terminal then send via UART then display on HEX
                mode_message = "Manually defined input";
                user_phrases_mode();
                break;
            case 2: // Accelerometer mode, read from accelerometer and display on HEX and on terminal
                mode_message = "Accelerometer mode";
                break;
            case 3:
                mode_message = "Mode 3";
                break;
            case 4: // Capture a single image and stored in SDRAM, then display the image on VGA
                mode_message = "Image capture mode";
                break;
            case 5: // CPU performance mode, run a CPU intensive task and display the time taken on HEX and on terminal
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
