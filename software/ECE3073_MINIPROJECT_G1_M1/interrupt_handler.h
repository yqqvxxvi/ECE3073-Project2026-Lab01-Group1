#ifndef INTERRUPT_HANDLER_H
#define INTERRUPT_HANDLER_H

#include <stdint.h>

#define UART_RX_BUF_SIZE 64

/* IRQ-driven flags - set by ISR, cleared by main loop */
extern volatile int ms_ticks;  /* incremented every 1 ms by timer ISR  */
extern volatile int timer_flag;     /* set on every timer tick              */
extern volatile int uart_flag;      /* set when UART characters received    */
extern volatile int key_flag;       /* bitmask confirmed after release: bit 0=KEY0, bit 1=KEY1 */
extern volatile int key_pending;    /* bitmask of keys pressed, awaiting release */
extern volatile int sw_changed;     /* set when slide switches change       */
extern volatile int gsensor_flag;   /* set when accelerometer fires         */

/* UART receive ring buffer */
extern volatile char uart_rx_buf[UART_RX_BUF_SIZE];
extern volatile int  uart_rx_head;
extern volatile int  uart_rx_tail;

void irq_init(void);
int  get_mode(void);
void key_process_debounce(void); /* call every main loop iteration to detect key release */
void uart_putchar(char c);       /* transmit one byte  - waits for tx ready              */
void uart_puts(const char *s);   /* transmit null-terminated string                      */
char uart_getchar(void);         /* blocking read from ISR ring buffer                   */

#endif /* INTERRUPT_HANDLER_H */
