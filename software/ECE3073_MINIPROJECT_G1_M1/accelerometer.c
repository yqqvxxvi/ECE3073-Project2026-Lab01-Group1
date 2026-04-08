#include "accelerometer.h"
#include "system.h"
#include <altera_avalon_spi.h>
#include <stdint.h>
#include <stdio.h>

/* =========================================================
 * SPI transfer helpers
 *
 * ADXL345 SPI frame (first byte):
 *   bit 7  : R/W   (1 = read, 0 = write)
 *   bit 6  : MB    (1 = multi-byte, 0 = single byte)
 *   bits 5-0: register address
 *
 * alt_avalon_spi_command keeps CS asserted across both the
 * write (command) phase and the read phase, which is required
 * for ADXL345 register reads.
 * ========================================================= */

uint8_t accelerometer_read_reg(uint8_t reg)
{
    uint8_t tx = ADXL345_READ | (reg & 0x3F);
    uint8_t rx = 0;
    alt_avalon_spi_command(SPI_0_BASE, ADXL345_SLAVE, 1, &tx, 1, &rx, 0);
    return rx;
}

void accelerometer_write_reg(uint8_t reg, uint8_t val)
{
    uint8_t tx[2] = { (uint8_t)(reg & 0x3F), val };   /* write: bit7 clear */
    alt_avalon_spi_command(SPI_0_BASE, ADXL345_SLAVE, 2, tx, 0, 0, 0);
}

/* =========================================================
 * accelerometer_clear_interrupt
 *
 * Reading INT_SOURCE clears ALL interrupt flags on the ADXL345.
 * Call this from the main loop (not the ISR) after gsensor_flag fires.
 * Returns the INT_SOURCE byte so the caller can distinguish
 * single-tap vs double-tap if needed.
 * ========================================================= */
uint8_t accelerometer_clear_interrupt(void)
{
    return accelerometer_read_reg(ADXL345_REG_INT_SOURCE);
}

/* =========================================================
 * accelerometer_read_xyz
 *
 * Reads all 6 data bytes in one multi-byte SPI transfer.
 * ADXL345 output is little-endian (low byte first).
 * ========================================================= */
void accelerometer_read_xyz(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t tx = ADXL345_READ | ADXL345_MULTIBYTE | ADXL345_REG_DATAX0;
    uint8_t rx[6] = {0};
    alt_avalon_spi_command(SPI_0_BASE, ADXL345_SLAVE, 1, &tx, 6, rx, 0);
    *x = (int16_t)((rx[1] << 8) | rx[0]);
    *y = (int16_t)((rx[3] << 8) | rx[2]);
    *z = (int16_t)((rx[5] << 8) | rx[4]);
}

/* =========================================================
 * accelerometer_init
 *
 * Configures the ADXL345 for single-tap and double-tap
 * detection on all axes, routed to the INT1 pin (which is
 * wired to PIO_9 / GSENSOR_INT on the DE-10).
 *
 * Key register values (see ADXL345 datasheet):
 *   THRESH_TAP : 0x20 = 32 * 62.5 mg = 2 g  (tap sensitivity)
 *   DUR        : 0x10 = 16 * 625 us  = 10 ms (max tap duration)
 *   LATENT     : 0x50 = 80 * 1.25 ms = 100 ms (post-tap quiet)
 *   WINDOW     : 0xFF = 255 * 1.25 ms ~= 319 ms (double-tap window)
 *   TAP_AXES   : 0x07 = X+Y+Z all active
 *   INT_MAP    : 0x00 = all interrupts routed to INT1
 *   INT_ENABLE : single-tap | double-tap bits
 *   POWER_CTL  : 0x08 = measurement mode
 * ========================================================= */
void accelerometer_init(void)
{
    /* Verify SPI is working: DEVID register always returns 0xE5 on ADXL345 */
    uint8_t devid = accelerometer_read_reg(ADXL345_REG_DEVID);
    printf("ADXL345 DEVID: 0x%02X (expect 0xE5)\n", devid);

    /* Standby mode before reconfiguring */
    accelerometer_write_reg(ADXL345_REG_POWER_CTL, 0x00);

    /* Tap detection axes: enable X, Y and Z */
    accelerometer_write_reg(ADXL345_REG_TAP_AXES, 0x07);

    /* Tap threshold: 3 g (0x30 = 48 * 62.5 mg) - firm tap required */
    accelerometer_write_reg(ADXL345_REG_THRESH_TAP, 0x30);

    /* Max tap duration: 20 ms (0x20 = 32 * 625 us) - widened to catch lighter taps */
    accelerometer_write_reg(ADXL345_REG_DUR, 0x20);

    /* Double-tap latency: 100 ms */
    accelerometer_write_reg(ADXL345_REG_LATENT, 0x50);

    /* Double-tap window: ~319 ms */
    accelerometer_write_reg(ADXL345_REG_WINDOW, 0xFF);

    /* Route all interrupts to INT1 (connected to PIO_9) */
    accelerometer_write_reg(ADXL345_REG_INT_MAP, 0x00);

    /* Clear any stale interrupt before enabling */
    accelerometer_clear_interrupt();

    /* Enable single-tap and double-tap interrupts */
    accelerometer_write_reg(ADXL345_REG_INT_ENABLE,
                            ADXL345_INT_SINGLE_TAP | ADXL345_INT_DOUBLE_TAP);

    /* Enter measurement mode */
    accelerometer_write_reg(ADXL345_REG_POWER_CTL, 0x08);
}
