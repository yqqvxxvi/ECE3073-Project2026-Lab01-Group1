#include "utils.h"
#include "interrupt_handler.h"  /* for extern volatile int ms_ticks */

/*
 * Busy-wait delay using the 1 ms hardware tick from timer_0 ISR.
 * Interrupts remain enabled during the wait, so ms_ticks keeps advancing.
 * Cast to uint32_t so subtraction wraps correctly after rollover.
 */
void delay_ms(uint32_t count)
{
    uint32_t start = (uint32_t)ms_ticks;
    while (((uint32_t)ms_ticks - start) < count)
    {
        /* busy wait */
    }
}
