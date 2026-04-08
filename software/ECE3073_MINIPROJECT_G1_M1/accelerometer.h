#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdint.h>

/* =========================================================
 * ADXL345 SPI protocol bits (first byte of every transfer)
 * ========================================================= */
#define ADXL345_READ      0x80  /* bit 7: 1 = read, 0 = write      */
#define ADXL345_MULTIBYTE 0x40  /* bit 6: 1 = multi-byte transfer   */

/* SPI slave index for alt_avalon_spi_command (0-indexed, not one-hot) */
#define ADXL345_SLAVE     0

/* =========================================================
 * ADXL345 register addresses
 * ========================================================= */
#define ADXL345_REG_DEVID        0x00
#define ADXL345_REG_THRESH_TAP   0x1D  /* tap threshold (62.5 mg/LSB)      */
#define ADXL345_REG_DUR          0x21  /* tap duration  (625 us/LSB)       */
#define ADXL345_REG_LATENT       0x22  /* double-tap latency (1.25 ms/LSB) */
#define ADXL345_REG_WINDOW       0x23  /* double-tap window  (1.25 ms/LSB) */
#define ADXL345_REG_TAP_AXES     0x2A  /* which axes participate in tap     */
#define ADXL345_REG_POWER_CTL    0x2D  /* power/measurement control         */
#define ADXL345_REG_INT_ENABLE   0x2E  /* interrupt enable mask             */
#define ADXL345_REG_INT_MAP      0x2F  /* 0=INT1 pin, 1=INT2 pin per bit   */
#define ADXL345_REG_INT_SOURCE   0x30  /* read to identify and clear IRQ   */
#define ADXL345_REG_DATA_FORMAT  0x31
#define ADXL345_REG_DATAX0       0x32  /* X low byte (multi-byte read base) */

/* =========================================================
 * INT_ENABLE / INT_SOURCE bit masks
 * ========================================================= */
#define ADXL345_INT_SINGLE_TAP  (1 << 6)
#define ADXL345_INT_DOUBLE_TAP  (1 << 5)

/* =========================================================
 * API
 * ========================================================= */

/* Configure ADXL345 for single/double tap interrupts on INT1. */
void accelerometer_init(void);

/* Read INT_SOURCE register to clear the interrupt on the chip. */
uint8_t accelerometer_clear_interrupt(void);

/* Read a single register. */
uint8_t accelerometer_read_reg(uint8_t reg);

/* Write a single register. */
void accelerometer_write_reg(uint8_t reg, uint8_t val);

/* Read all three axes (6 bytes, multi-byte SPI transfer). */
void accelerometer_read_xyz(int16_t *x, int16_t *y, int16_t *z);

#endif /* ACCELEROMETER_H */
