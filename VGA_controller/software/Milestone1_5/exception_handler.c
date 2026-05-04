/*
 * Reset trampoline.
 *
 * BSP is built with hal.linker.allow_code_at_reset = 1, so the .reset
 * section content must be supplied by user code. The CPU starts at
 * 0x04000000 and falls into the_reset(), which jumps to main().
 *
 * Exception handling is left to the BSP HAL; nothing in this file goes
 * into the .exceptions section.
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
