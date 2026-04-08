#include "system.h"
#include "nios2.h"          /* NIOS2_READ_IPENDING, NIOS2_WRITE_IENABLE, etc. */
#include "sys/alt_irq.h"    /* alt_irq_register */
#include "interrupt_handler.h"
#include "io.h"

/* =========================================================
 * Fallback base addresses
 * ========================================================= */
#ifndef TIMER_0_BASE
#define TIMER_0_BASE   0x4001000
#endif
#ifndef TIMER_0_IRQ
#define TIMER_0_IRQ    0
#endif
#ifndef TIMER_0_PERIOD
#define TIMER_0_PERIOD 49999   /* 50 MHz / 1 kHz - 1 */
#endif
#ifndef PIO_9_BASE
#define PIO_9_BASE     0x4001040
#endif
#ifndef PIO_9_IRQ
#define PIO_9_IRQ      2
#endif
#ifndef PIO_8_BASE
#define PIO_8_BASE     0x4001050
#endif
#ifndef PIO_8_IRQ
#define PIO_8_IRQ      3
#endif
#ifndef PIO_7_BASE
#define PIO_7_BASE     0x4001060
#endif
#ifndef PIO_7_IRQ
#define PIO_7_IRQ      4
#endif

/* =========================================================
 * Avalon PIO register offsets
 *   0x00  Data       (read input value)
 *   0x08  IRQ mask   (1 = enable IRQ on that bit)
 *   0x0C  Edge capture (write any value to clear)
 * Avalon Timer register offsets
 *   0x00  Status  (bit 0 = TO timeout flag; write 0 to clear)
 *   0x04  Control (bit 0=ITO, bit 1=CONT, bit 2=START, bit 3=STOP)
 *   0x08  PeriodL (lower 16 bits)
 *   0x0C  PeriodH (upper 16 bits)
 * ========================================================= */

/* Timer_0 */
static volatile int *Timer_Status  = (volatile int *)(TIMER_0_BASE + 0x00);
static volatile int *Timer_Control = (volatile int *)(TIMER_0_BASE + 0x04);
static volatile int *Timer_PeriodL = (volatile int *)(TIMER_0_BASE + 0x08);
static volatile int *Timer_PeriodH = (volatile int *)(TIMER_0_BASE + 0x0C);

/* PIO_8 - Keys (2-bit, falling edge) */
static volatile int *Keys_Data    = (volatile int *)(PIO_8_BASE + 0x00);
static volatile int *Keys_IRQMask = (volatile int *)(PIO_8_BASE + 0x08);
static volatile int *Keys_EdgeCap = (volatile int *)(PIO_8_BASE + 0x0C);

/* PIO_7 - Slide switches (10-bit, any edge) */
static volatile int *SW_IRQMask   = (volatile int *)(PIO_7_BASE + 0x08);
static volatile int *SW_EdgeCap   = (volatile int *)(PIO_7_BASE + 0x0C);

/* PIO_9 - GSENSOR_INT (1-bit, rising edge) */
static volatile int *Gsensor_IRQMask = (volatile int *)(PIO_9_BASE + 0x08);
static volatile int *Gsensor_EdgeCap = (volatile int *)(PIO_9_BASE + 0x0C);

/* =========================================================
 * Global flags - set in ISR, read and cleared in main loop.
 * Never call SPI or printf from inside an ISR.
 * ========================================================= */
volatile int ms_ticks    = 0;
volatile int key_flag    = 0;   /* bitmask: bit0=KEY0, bit1=KEY1 */
volatile int sw_changed  = 0;   /* any slide switch toggled */
volatile int gsensor_flag = 0;  /* accelerometer tap detected */

/* =========================================================
 * Individual ISRs registered with the HAL.
 * The HAL exception handler calls these after saving context.
 * Signature required by alt_irq_register:
 *   void isr(void *context, alt_u32 id)
 * ========================================================= */

static void timer_isr(void *context, alt_u32 id)
{
    (void)context; (void)id;
    *Timer_Status = 0;   /* clear timeout flag */
    ms_ticks++;
}

static void keys_isr(void *context, alt_u32 id)
{
    (void)context; (void)id;
    /* Keys are active-low; invert edge capture bits to get which key was pressed */
    key_flag |= (*Keys_Data ^ 0x3) & 0x3;
    *Keys_EdgeCap = 0x3;   /* clear both edge capture bits */
}

static void sw_isr(void *context, alt_u32 id)
{
    (void)context; (void)id;
    sw_changed = 1;
    *SW_EdgeCap = 0x3FF;   /* clear all 10 edge capture bits */
}

static void gsensor_isr(void *context, alt_u32 id)
{
    (void)context; (void)id;
    gsensor_flag = 1;
    *Gsensor_EdgeCap = 0x1;   /* clear bit 0 edge capture */
}

/* =========================================================
 * irq_init - call once from main() before the main loop.
 * 1. Configures timer_0 for 1 ms continuous interrupt.
 * 2. Enables PIO IRQ masks for keys, switches, and gsensor.
 * 3. Registers each ISR with the HAL (alt_irq_register).
 *    The HAL enables global interrupts automatically.
 * ========================================================= */
void irq_init(void)
{
    /* --- Timer_0: 1 ms continuous interrupt --- */
    *Timer_Control = 0x8;                              /* STOP before reconfiguring */
    *Timer_Status  = 0;                                /* clear any pending timeout */
    *Timer_PeriodL = (int)(TIMER_0_PERIOD & 0xFFFF);
    *Timer_PeriodH = (int)((TIMER_0_PERIOD >> 16) & 0xFFFF);
    *Timer_Control = 0x7;                              /* ITO | CONT | START */

    /* --- PIO_8: Keys - enable IRQ on both bits --- */
    *Keys_EdgeCap  = 0x3;
    *Keys_IRQMask  = 0x3;

    /* --- PIO_7: Slide switches - enable IRQ on all 10 bits --- */
    *SW_EdgeCap    = 0x3FF;
    *SW_IRQMask    = 0x3FF;

    /* --- PIO_9: GSENSOR_INT - enable IRQ on bit 0 --- */
    *Gsensor_EdgeCap = 0x1;
    *Gsensor_IRQMask = 0x1;

    /* --- Register ISRs with the HAL interrupt controller --- */
    alt_irq_register(TIMER_0_IRQ, NULL, timer_isr);
    alt_irq_register(PIO_8_IRQ,   NULL, keys_isr);
    alt_irq_register(PIO_7_IRQ,   NULL, sw_isr);
    alt_irq_register(PIO_9_IRQ,   NULL, gsensor_isr);
}
