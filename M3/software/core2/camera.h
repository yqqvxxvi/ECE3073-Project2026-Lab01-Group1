/*
 * camera.h
 *
 * ESP32 camera interface over SPI for the Nios II Group 1 project.
 *
 * Protocol (ESP32 -> Nios II, 786 bytes per capture):
 *
 *   [0xAA] [0x55]                : 2-byte sync header
 *   [pixel_0 ... pixel_783]      : 28x28 grayscale image, 0..255 per pixel
 *
 * Transport: shares SPI_0 with the water meter, different SS line.
 * Switching mode is controlled by sw3 in the main loop.
 *
 * Receive strategy:
 *   - Triggered by KEY0 press while in camera mode (sw3 == 1).
 *   - Camera capture pulls a fresh 786-byte frame via SPI interrupts.
 *   - When complete, ISR sets camera_frame_ready = 1.
 *   - Main loop calls camera_render_to_dashboard() to paint into the
 *     VGA monitor box.
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <stdint.h>

extern volatile int camera_mode_active;
extern volatile int camera_capture_requested;
extern volatile int camera_frame_ready;

void camera_init(void);
void camera_enter_mode(void);
void camera_leave_mode(void);
void camera_trigger_capture(void);
void camera_spi_isr_handler(void);
void camera_render_to_dashboard(void);
void camera_debug_print_frame(void);

#endif /* CAMERA_H_ */
