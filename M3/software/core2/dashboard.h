/*
 * dashboard.h
 *
 * VGA dashboard for the Nios II Group 1 project.
 *
 * Layout (all coordinates in 320x240 framebuffer space, hardware
 * upscales to 640x320):
 *
 *   Title bar       : y 0..15
 *   VGA monitor     : x 2..158,   y 18..118  (camera preview + scan line)
 *   Accelerometer   : x 160..318, y 18..58
 *   System status   : x 160..318, y 62..118
 *   LEDs            : x 2..158,   y 122..158
 *   Switches        : x 160..318, y 122..158
 *   Pushbuttons     : x 2..158,   y 162..198
 *   HEX displays    : x 160..318, y 162..198
 *
 * Drawing strategy:
 *   - dashboard_init_static() composes the static frame in the SDRAM
 *     scratch buffer and pushes it to the VGA controller ONCE.
 *   - All dashboard_update_*() functions write directly to the VGA
 *     controller via write_pixel(), bypassing SDRAM. They only repaint
 *     the small dynamic region they own.
 *
 * Do NOT call display_sdram_image() after init, or it will clobber
 * every dynamic update.
 */

#ifndef DASHBOARD_H_
#define DASHBOARD_H_

#include <stdint.h>

/* ---------------- Public API ---------------- */

/*
 * Build the static frame in SDRAM and push it to the VGA controller.
 * Call once at startup, before any dashboard_update_*() calls.
 */
void dashboard_init_static(void);

/*
 * Update individual dynamic regions. Each repaints only its own area.
 * Safe to call every main loop iteration.
 */
void dashboard_update_leds(uint8_t led_state);
void dashboard_update_switches(uint8_t sw_state);
void dashboard_update_keys(int key0_pressed, int key1_pressed);
void dashboard_update_accel(int pitch, int roll, int z, int tilt_safe);
void dashboard_update_status(int spi_running,
                             int emergency,
                             const char *reading_5digit);
void dashboard_update_hex(const uint8_t hex_digits[6]);

/*
 * Advance the camera scan line one row inside the VGA monitor box.
 * Call this every N main loop iterations to control sweep speed.
 * When the line reaches the bottom of the inner canvas it wraps to
 * the top.
 */
void dashboard_tick_scanline(void);

#endif /* DASHBOARD_H_ */
