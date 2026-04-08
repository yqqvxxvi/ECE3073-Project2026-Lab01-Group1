#ifndef INTERRUPT_HANDLER_H
#define INTERRUPT_HANDLER_H

#include <stdint.h>

/*
 * Global flags set by ISR, consumed by main loop.
 * Read and clear these in main; never call SPI or printf from the ISR.
 */
extern volatile int ms_ticks;    /* 1 ms hardware tick counter (timer_0 IRQ) */
extern volatile int key_flag;    /* bitmask: which KEY was pressed (PIO_8 edge capture) */
extern volatile int sw_changed;  /* set when any slide switch changes (PIO_7 edge) */
extern volatile int gsensor_flag;/* set when accelerometer asserts INT1 (PIO_9 edge) */

/*
 * Call irq_init() once from main() before enabling global interrupts.
 * It configures timer_0 for 1 ms, enables PIO IRQ masks, and calls
 * NIOS2_WRITE_IENABLE / NIOS2_WRITE_STATUS(1).
 */
void irq_init(void);


#endif /* INTERRUPT_HANDLER_H */
