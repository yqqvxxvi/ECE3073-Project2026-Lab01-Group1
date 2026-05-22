#ifndef VGA_CUSTOM_H
#define VGA_CUSTOM_H

#include <stdint.h>

#define VGA_PIX_WIDTH   320
#define VGA_PIX_HEIGHT  240

/*
 * Memory-mapped exports for the custom VGA controller.
 *
 * ADDR_EXPORT  : pixel linear address  (17 bits, 0..76799)
 * PIXEL_EXPORT : pixel colour          (4  bits)
 * WREN_EXPORT  : pulse-high write strobe (1 bit)
 *
 * The current Qsys does not yet name these exports. Until the BSP is
 * regenerated with the new PIO names, these placeholder addresses live
 * in the same 0x080090xx range as the existing SPI peripherals so that
 * the project at least links. Replace with the real PIO bases (e.g.
 * VGA_ADDR_BASE / VGA_PIXEL_BASE / VGA_WREN_BASE) once Qsys is updated.
 */
extern volatile unsigned int *ADDR_EXPORT;
extern volatile unsigned int *PIXEL_EXPORT;
extern volatile unsigned int *WREN_EXPORT;

void write_pixel(unsigned int addr, unsigned char color);
void draw_color_stripes(void);
void draw_vertical_stripes(void);

#endif
