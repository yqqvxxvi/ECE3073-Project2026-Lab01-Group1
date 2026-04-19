#include <stdio.h>
#include <includes.h>
#include <stdint.h>
#include <string.h>
#include <system.h>
#include <io.h>

#define TASK1_STACKSIZE 1024
#define TASK1_PRIORITY  5

OS_STK task1_stk[TASK1_STACKSIZE];

volatile int * SPI_RxData  = (int *) 0x04009000;
volatile int * SPI_TxData  = (int *) 0x04009004;
volatile int * SPI_Status  = (int *) 0x04009008;
volatile int * SPI_Control = (int *) 0x0400900C;

volatile int * SPI_SS      = (int *) 0x040090A0;
volatile int * OutPort_LED = (int *) 0x040090C0;
volatile int * green_LED   = (int *) 0x04009090;
volatile int * speaker     = (int *) 0x04009080;

#define TRDY 0x40
#define RRDY 0x80

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

void task1(void *pdata)
{
    char message[64];
    int index = 0;
    uint8_t rx;

    (void)pdata;

    *SPI_SS = 0x1;
    *OutPort_LED = 0x00;
    *green_LED = 0x00;   // OFF initially (active-high)
    *speaker   = 0x00;   // OFF initially

    printf("START SPI STRING RECEIVE\n");

    while (1) {
        rx = spi_transfer(0xFF);

        printf("Received byte: 0x%02X", rx);
        if (rx >= 32 && rx <= 126) {
            printf(" ('%c')", rx);
        }
        printf("\n");

        if (rx == '\0') {
            message[index] = '\0';
            printf("Full sentence: %s\n", message);
            printf("-------------------------\n");

            *speaker     = 0x00;   // OFF
            *OutPort_LED = 0x01;   // ON
            *green_LED   = 0x01;   // ON

            OSTimeDlyHMSM(0, 0, 1, 0);

            *OutPort_LED = 0x00;   // OFF
            *green_LED   = 0x00;   // OFF

            index = 0;
        } else {
            *green_LED = 0x00;     // keep OFF until full sentence

            if (index < (int)(sizeof(message) - 1)) {
                message[index++] = (char)rx;

                // Short audible beep burst while bytes are coming in
                beep_cycles(80, 60);
            } else {
                message[sizeof(message) - 1] = '\0';
                printf("Buffer overflow, partial sentence: %s\n", message);

                index = 0;
                *speaker   = 0x00; // OFF
                *green_LED = 0x00; // OFF
            }
        }

        OSTimeDlyHMSM(0, 0, 0, 100);
    }
}

int main(void)
{
    OSInit();

    printf("============================\n");

    OSTaskCreateExt(task1,
                    NULL,
                    (void *)&task1_stk[TASK1_STACKSIZE - 1],
                    TASK1_PRIORITY,
                    TASK1_PRIORITY,
                    task1_stk,
                    TASK1_STACKSIZE,
                    NULL,
                    0);

    OSStart();
    return 0;
}
