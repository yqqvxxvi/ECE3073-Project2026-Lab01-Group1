#include "nios2_ctrl_reg_macros.h"
#include "system.h"
#include "interrupt_handler.h"
#include <stdint.h>

/* function prototypes */
void main(void);
void interrupt_handler(void);
void the_exception(void);

/* ------------------------------------------------------------------
 * Global flags - set in ISR, polled and cleared by main loop
 * ------------------------------------------------------------------ */
volatile int      key_flag     = 0;  /* bitmask of keys confirmed released after press */
volatile int      sw_changed   = 0;
volatile int      gsensor_flag = 0;  /* kept for main.c compatibility, unused by ISR  */
volatile int      uart_flag    = 0;
volatile int      timer_flag   = 0;
volatile int      ms_ticks     = 0;  /* incremented every 1 ms by timer ISR           */

/* Key debounce state: ISR records a press here; main calls
 * key_process_debounce() to wait for release before setting key_flag */
volatile int key_pending  = 0;

/* UART receive ring buffer */
volatile char uart_rx_buf[UART_RX_BUF_SIZE];
volatile int  uart_rx_head = 0;
volatile int  uart_rx_tail = 0;

/* ------------------------------------------------------------------
 * Hardware register pointers
 *
 * altera_avalon_uart (UART_0) register map:
 *   +0x00  rxdata  [7:0]  received byte (valid when status.RRDY=1)
 *   +0x04  txdata  [7:0]  transmit byte
 *   +0x08  status  bit7=RRDY (rx ready), bit6=TRDY (tx ready)
 *   +0x0C  control bit7=IRRDY (enable rx-ready interrupt)
 *
 * altera_avalon_timer (TIMER_0) register map:
 *   +0x00  status  bit0=TO (timeout) - write 0 to clear
 *   +0x04  control
 *
 * altera_avalon_pio edge-capture/IRQ-mask:
 *   base+0x08  interruptmask
 *   base+0x0C  edgecapture
 * ------------------------------------------------------------------ */
/* Nios UART register map (Table 1 of ds_nios_uart):
 *   +0x00  rxdata   RO  received byte     (reading clears status.rrdy)
 *   +0x04  txdata   WO  byte to transmit  (write only when status.trdy=1)
 *   +0x08  status   RW  bit7=rrdy, bit6=trdy, bit3=roe, bit5=tmt, ...
 *                       write any value to clear error bits
 *   +0x0C  control  RW  bit7=irrdy (enable rx-ready IRQ), bit6=itrdy, ... */
static volatile int *UART_RXDATA  = (volatile int *)(UART_0_BASE + 0x00);
static volatile int *UART_TXDATA  = (volatile int *)(UART_0_BASE + 0x04);
static volatile int *UART_STATUS  = (volatile int *)(UART_0_BASE + 0x08);
static volatile int *UART_CTRL    = (volatile int *)(UART_0_BASE + 0x0C);

/* altera_avalon_jtag_uart register map:
 *   +0x00 data    bits[7:0]=char, bit15=RVALID, bits[31:16]=ravail
 *   +0x04 control bits[31:16]=wspace (write FIFO space available)
 * Writing to data when wspace>0 sends a char to the Nios II console */
static volatile int *JTAG_UART_DATA = (volatile int *)(JTAG_UART_0_BASE + 0x00);
static volatile int *JTAG_UART_CTRL = (volatile int *)(JTAG_UART_0_BASE + 0x04);

/* altera_avalon_timer register map:
 *   +0x00 status  bit0=TO (timeout) - write any value to clear
 *   +0x04 control bit0=ITO (IRQ on timeout), bit1=CONT (continuous), bit2=START
 *   +0x08 periodl low  16 bits of reload period
 *   +0x0C periodh high 16 bits of reload period
 * TIMER_0_PERIOD = 49999 counts at 50 MHz = 1 ms per tick */
static volatile int *TIMER_STATUS  = (volatile int *)(TIMER_0_BASE + 0x00);
static volatile int *TIMER_CONTROL = (volatile int *)(TIMER_0_BASE + 0x04);
static volatile int *TIMER_PERIODL = (volatile int *)(TIMER_0_BASE + 0x08);
static volatile int *TIMER_PERIODH = (volatile int *)(TIMER_0_BASE + 0x0C);

static volatile int *KEYS_EDGE_CAP = (volatile int *)(PIO_8_BASE + 0x0C);
static volatile int *KEYS_IRQ_MASK = (volatile int *)(PIO_8_BASE + 0x08);
static volatile int *KEYS_DATA     = (volatile int *)(PIO_8_BASE + 0x00);

static volatile int *SW_DATA       = (volatile int *)(PIO_7_BASE + 0x00);

/* ------------------------------------------------------------------
 * CPU reset vector
 * ------------------------------------------------------------------ */
void the_reset(void) __attribute__((section(".reset")));
void the_reset(void)
{
    asm(".set noat");
    asm(".set nobreak");
    asm("movia r2, main");
    asm("jmp r2");
}

/* ------------------------------------------------------------------
 * CPU exception vector - do not modify
 * ------------------------------------------------------------------ */
void the_exception(void) __attribute__((section(".exceptions")));
void the_exception(void)
{
    asm(".set noat");
    asm(".set nobreak");
    asm("subi  sp, sp, 128");
    asm("stw   et, 96(sp)");
    asm("rdctl et, ctl4");
    asm("beq   et, r0, SKIP_EA_DEC");
    asm("subi  ea, ea, 4");
    asm("SKIP_EA_DEC:");
    asm("stw   r1,  4(sp)");
    asm("stw   r2,  8(sp)");
    asm("stw   r3,  12(sp)");
    asm("stw   r4,  16(sp)");
    asm("stw   r5,  20(sp)");
    asm("stw   r6,  24(sp)");
    asm("stw   r7,  28(sp)");
    asm("stw   r8,  32(sp)");
    asm("stw   r9,  36(sp)");
    asm("stw   r10, 40(sp)");
    asm("stw   r11, 44(sp)");
    asm("stw   r12, 48(sp)");
    asm("stw   r13, 52(sp)");
    asm("stw   r14, 56(sp)");
    asm("stw   r15, 60(sp)");
    asm("stw   r16, 64(sp)");
    asm("stw   r17, 68(sp)");
    asm("stw   r18, 72(sp)");
    asm("stw   r19, 76(sp)");
    asm("stw   r20, 80(sp)");
    asm("stw   r21, 84(sp)");
    asm("stw   r22, 88(sp)");
    asm("stw   r23, 92(sp)");
    asm("stw   r25, 100(sp)");
    asm("stw   r26, 104(sp)");
    asm("stw   r28, 112(sp)");
    asm("stw   r29, 116(sp)");
    asm("stw   r30, 120(sp)");
    asm("stw   r31, 124(sp)");
    asm("addi  fp, sp, 128");
    asm("call  interrupt_handler");
    asm("ldw   r1,  4(sp)");
    asm("ldw   r2,  8(sp)");
    asm("ldw   r3,  12(sp)");
    asm("ldw   r4,  16(sp)");
    asm("ldw   r5,  20(sp)");
    asm("ldw   r6,  24(sp)");
    asm("ldw   r7,  28(sp)");
    asm("ldw   r8,  32(sp)");
    asm("ldw   r9,  36(sp)");
    asm("ldw   r10, 40(sp)");
    asm("ldw   r11, 44(sp)");
    asm("ldw   r12, 48(sp)");
    asm("ldw   r13, 52(sp)");
    asm("ldw   r14, 56(sp)");
    asm("ldw   r15, 60(sp)");
    asm("ldw   r16, 64(sp)");
    asm("ldw   r17, 68(sp)");
    asm("ldw   r18, 72(sp)");
    asm("ldw   r19, 76(sp)");
    asm("ldw   r20, 80(sp)");
    asm("ldw   r21, 84(sp)");
    asm("ldw   r22, 88(sp)");
    asm("ldw   r23, 92(sp)");
    asm("ldw   r24, 96(sp)");
    asm("ldw   r25, 100(sp)");
    asm("ldw   r26, 104(sp)");
    asm("ldw   r28, 112(sp)");
    asm("ldw   r29, 116(sp)");
    asm("ldw   r30, 120(sp)");
    asm("ldw   r31, 124(sp)");
    asm("addi  sp, sp, 128");
    asm("eret");
}

/* ------------------------------------------------------------------
 * irq_init - configure peripherals and enable NIOS II interrupts
 *
 * IRQ map:
 *   IRQ 0 - TIMER_0     (bit 0 = 0x01)
 *   IRQ 1 - UART_0      (bit 1 = 0x02)
 *   IRQ 3 - PIO_8 keys  (bit 3 = 0x08)
 *   IRQ 4 - PIO_7 sw    (bit 4 = 0x10)
 * ------------------------------------------------------------------ */
void irq_init(void)
{
    uart_puts("\r\n=== SYSTEM INIT ===\r\n");

    /* UART_0: enable receive-ready interrupt (IRRDY = bit 7 of control) */
    *UART_CTRL |= 0x80;
    uart_puts("UART_0 rx IRQ : ");
    uart_puts((*UART_STATUS & 0x40) ? "OK\r\n" : "FAILED\r\n");

    /* TIMER_0: load 1 ms period (49999 counts at 50 MHz), run continuously */
    *TIMER_PERIODL = TIMER_0_PERIOD & 0xFFFF;
    *TIMER_PERIODH = (TIMER_0_PERIOD >> 16) & 0xFFFF;
    *TIMER_STATUS  = 0;
    *TIMER_CONTROL = 0x7;        /* ITO=1, CONT=1, START=1 */
    uart_puts("Timer  1ms    : ");
    uart_puts((*TIMER_CONTROL & 0x1) ? "OK\r\n" : "FAILED\r\n");

    /* Keys / PIO_8: clear any stale edge captures, enable IRQ for KEY0+KEY1 */
    *KEYS_EDGE_CAP = 0x3;
    *KEYS_IRQ_MASK = 0x3;
    uart_puts("Keys   IRQ    : ");
    uart_puts((*KEYS_IRQ_MASK == 0x3) ? "OK\r\n" : "FAILED\r\n");

    /* Switches / PIO_7: polled directly via get_mode(), no IRQ needed */
    uart_puts("SW     polled : OK\r\n");

    /* Enable IRQ lines 0,1,3 and set global interrupt enable (PIE) */
    NIOS2_WRITE_IENABLE(0x0B);  /* bits 0,1,3 = timer, UART, keys */
    NIOS2_WRITE_STATUS(0x1);
    uart_puts("===================\r\n\r\n");
}

/* ------------------------------------------------------------------
 * uart_putchar - transmit one byte via UART_0
 *
 * Per datasheet: must wait for trdy (status bit 6) = 1 before
 * writing txdata. Writing while trdy=0 causes a transmit overrun.
 * ------------------------------------------------------------------ */
void uart_putchar(char c)
{
    /* Send to hardware UART_0 */
    while (!(*UART_STATUS & 0x40)) /* spin until trdy=1 */
        ;
    *UART_TXDATA = (int)c;

    /* Mirror to JTAG UART (non-blocking: drop char if FIFO full, never hang) */
    if ((*JTAG_UART_CTRL) >> 16)
        *JTAG_UART_DATA = (unsigned char)c;
}

/* ------------------------------------------------------------------
 * uart_puts - transmit a null-terminated string via UART_0
 * ------------------------------------------------------------------ */
void uart_puts(const char *s)
{
    while (*s)
        uart_putchar(*s++);
}

/* ------------------------------------------------------------------
 * uart_getchar - blocking read from the ISR ring buffer
 *
 * Spins until the ISR has placed at least one character in the
 * buffer, then consumes and returns it.
 * ------------------------------------------------------------------ */
char uart_getchar(void)
{
    while (uart_rx_head == uart_rx_tail)
        ; /* wait for ISR to push a character */
    char c = uart_rx_buf[uart_rx_tail];
    uart_rx_tail = (uart_rx_tail + 1) % UART_RX_BUF_SIZE;
    return c;
}

/* ------------------------------------------------------------------
 * key_process_debounce
 *
 * Call this every iteration of the main loop.
 * The ISR records a press in key_pending. Here we wait until all
 * pressed keys are released (bits go high, active-low) before
 * committing the event to key_flag.
 * ------------------------------------------------------------------ */
void key_process_debounce(void)
{
    if (!key_pending)
        return;

    /* Active-low: released = bit is 1. Wait until every pending key
     * shows as released in the current DATA register. */
    if ((*KEYS_DATA & key_pending) == key_pending)
    {
        key_flag  |= key_pending;
        key_pending = 0;
    }
}

/* ------------------------------------------------------------------
 * Interrupt Service Routine
 * ------------------------------------------------------------------ */
void interrupt_handler(void)
{
    int ipending;
    NIOS2_READ_IPENDING(ipending);

    /* -- TIMER_0 (IRQ 0) -------------------------------------------- */
    if (ipending & 0x1)
    {
        *TIMER_STATUS = 0; /* write any value to clear TO flag */
        ms_ticks++;
        timer_flag = 1;
    }

    /* -- UART_0 receive (IRQ 1) -------------------------------------- */
    if (ipending & 0x2)
    {
        int status = *UART_STATUS;

        /* roe (bit 3): overrun - previous char was overwritten.
         * Clear all error bits by writing any value to status register. */
        if (status & 0x8)
            *UART_STATUS = 0;

        /* rrdy (bit 7): a received byte is waiting in rxdata.
         * The UART has no FIFO - one holding register only.
         * Reading rxdata clears rrdy and de-asserts the IRQ. */
        if (status & 0x80)
        {
            char c = (char)(*UART_RXDATA & 0xFF);
            int next = (uart_rx_head + 1) % UART_RX_BUF_SIZE;
            if (next != uart_rx_tail) /* drop if buffer full */
            {
                uart_rx_buf[uart_rx_head] = c;
                uart_rx_head = next;
            }
            uart_flag = 1;
        }
    }

    /* -- Keys / PIO_8 (IRQ 3) --------------------------------------- */
    if (ipending & 0x8)
    {
        /* Record which keys were pressed; main loop detects release */
        key_pending |= *KEYS_EDGE_CAP & 0x3;
        *KEYS_EDGE_CAP = 0x3; /* clear to de-assert the IRQ line */
    }

}
