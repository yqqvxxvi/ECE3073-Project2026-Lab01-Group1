/*
 * Reset trampoline.
 *
 * The BSP is configured with hal.linker.allow_code_at_reset = 1, so the BSP
 * does NOT provide a default .reset section. This file supplies the minimum
 * code the CPU needs at the reset vector to jump into main().
 *
 * Exception handling is left to the BSP (alt_exception_entry from
 * libhal_bsp.a fills .exceptions.entry.*), so alt_irq_register-based ISRs
 * (e.g. spi_isr in SPI_INTERRUPT.c) work normally.
 */

void main(void);

void the_reset(void) __attribute__((section(".reset")));
void the_reset(void)
{
    asm(".set noat");
    asm(".set nobreak");
    asm("movia r2, main");
    asm("jmp r2");
}
