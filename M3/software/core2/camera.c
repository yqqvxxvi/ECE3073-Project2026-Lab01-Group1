/*
 * camera.c
 *
 * SPI receive + VGA paint for the ESP32-C3 + Grove Vision AI V2.
 *
 * Request/response protocol:
 *   Nios II:  sends [0xC0]        (capture command)
 *   ESP32:    starts capture asynchronously, queues frame when ready
 *   Nios II:  keeps clocking 0x00 bytes, scanning for [0xAA 0x55] header
 *   ESP32:    sends [0xAA] [0x55] [784 grayscale bytes] when ready
 *
 * The ISR state machine syncs on the header, so any pre-header filler
 * bytes from the ESP32 are harmless.
 *
 * Shares SPI_0 with the water meter. Mode is selected by
 * camera_mode_active. The water meter ISR (spi_isr in main.c) should
 * check camera_mode_active and delegate to camera_spi_isr_handler() when
 * it is set.
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "system.h"
#include <io.h>
#include <sys/alt_irq.h>

#include "camera.h"

/* Defined in the main translation unit. */
extern volatile int *SPI_RxData;
extern volatile int *SPI_TxData;
extern volatile int *SPI_Status;
extern volatile int *SPI_Control;
extern volatile int *SPI_SS;

extern volatile int spi_running;
extern volatile int spi_next_byte_needed;
extern volatile int spi_byte_ready;

extern void short_delay(volatile int count);
extern void spi_flush_rx(void);
extern void spi_start_one_byte_transfer(uint8_t tx);
extern void write_pixel(unsigned int addr, unsigned char color);

extern const uint8_t font5x7[37][7];
extern int  get_font_index(char c);

#define TRDY 0x40
#define RRDY 0x80

/* ---------------- Protocol constants ---------------- */
#define CAM_CMD_CAPTURE     0xC0
#define CAM_HDR_0           0xAA
#define CAM_HDR_1           0x55
#define CAM_IMG_W           28
#define CAM_IMG_H           28
#define CAM_IMG_BYTES       (CAM_IMG_W * CAM_IMG_H)        /* 784 */
#define CAM_FRAME_BYTES     (2 + CAM_IMG_BYTES)            /* 786 */

/* Max filler bytes we'll clock before giving up on the response. */
#define CAM_MAX_POLL_BYTES  4000

/* SS polarity matches the water meter:
 *   0x0 = slave selected (active)
 *   0x1 = slave deselected (idle)
 * If you actually have two separate SS lines and the camera is on
 * SS[1], change CAM_SS_SELECT to 0x2 and CAM_SS_DESELECT to 0x3.
 */
#define CAM_SS_SELECT       0x0
#define CAM_SS_DESELECT     0x1

typedef enum {
    CAM_RX_IDLE,
    CAM_RX_SYNC,        /* waiting for header byte 0 */
    CAM_RX_HDR1,        /* waiting for header byte 1 */
    CAM_RX_IMG,         /* receiving 784 image bytes */
    CAM_RX_DONE
} cam_rx_state_t;

/* ---------------- State ---------------- */

volatile int camera_mode_active       = 0;
volatile int camera_capture_requested = 0;
volatile int camera_frame_ready       = 0;

static volatile cam_rx_state_t cam_state = CAM_RX_IDLE;
static volatile int  cam_img_idx         = 0;
static volatile int  cam_poll_count      = 0;
static uint8_t       cam_img_buf[CAM_IMG_BYTES];

/* Diagnostic: log the first few RX bytes per capture so we can see
 * what's actually arriving on MISO. */
#define CAM_FIRST_RX_LOG 16
static volatile uint8_t cam_first_rx_log[CAM_FIRST_RX_LOG];
static volatile int     cam_first_rx_count = 0;

/* VGA monitor inner canvas (from dashboard layout). */
#define MON_INNER_X0    6
#define MON_INNER_Y0    30
#define MON_INNER_X1    154
#define MON_INNER_Y1    114

#define IMG_SCALE       2
#define IMG_W_PX        (CAM_IMG_W * IMG_SCALE)
#define IMG_H_PX        (CAM_IMG_H * IMG_SCALE)
#define IMG_X0          (MON_INNER_X0 + ((MON_INNER_X1 - MON_INNER_X0 + 1) - IMG_W_PX) / 2)
#define IMG_Y0          (MON_INNER_Y0 + ((MON_INNER_Y1 - MON_INNER_Y0 + 1) - IMG_H_PX) / 2)

/* ---------------- Small VGA helpers (local, since dashboard.c keeps its
 * helpers static). ---------------- */

static void cam_px(int x, int y, unsigned char color)
{
    if (x < 0 || x >= 320 || y < 0 || y >= 240) return;
    write_pixel((unsigned int)(y * 320 + x), color);
}

static void cam_fill_rect(int x0, int y0, int x1, int y1, unsigned char color)
{
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            cam_px(x, y, color);
        }
    }
}

static void cam_draw_char(int x, int y, char c, unsigned char color, int scale)
{
    int idx = get_font_index(c);
    for (int row = 0; row < 7; row++) {
        uint8_t row_data = font5x7[idx][row];
        for (int col = 0; col < 5; col++) {
            int bit = (row_data >> (4 - col)) & 0x01;
            if (bit) {
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        cam_px(x + col * scale + sx,
                               y + row * scale + sy, color);
                    }
                }
            }
        }
    }
}

static void cam_draw_string(int x, int y, const char *s,
                            unsigned char color, int scale)
{
    int cx = x;
    int char_w = 5 * scale;
    int spacing = scale;
    while (*s) {
        cam_draw_char(cx, y, *s, color, scale);
        cx += char_w + spacing;
        s++;
    }
}

static void show_status(const char *msg, unsigned char color)
{
    /* Clear a strip near the top of the monitor inner canvas. */
    cam_fill_rect(MON_INNER_X0 + 1, MON_INNER_Y0 + 1,
                  MON_INNER_X1 - 1, MON_INNER_Y0 + 10, 0x0);
    cam_draw_string(MON_INNER_X0 + 4, MON_INNER_Y0 + 2, msg, color, 1);
}

static inline unsigned char map_gray_to_vga(uint8_t gray)
{
    /* [Inference] assumes VGA palette indices 0..F form a grayscale ramp. */
    return (unsigned char)((gray >> 4) & 0xF);
}

/* ---------------- Public ---------------- */

void camera_init(void)
{
    cam_state     = CAM_RX_IDLE;
    cam_img_idx   = 0;
    cam_poll_count = 0;
    memset(cam_img_buf, 0, sizeof(cam_img_buf));
}

void camera_enter_mode(void)
{
    spi_running          = 0;
    spi_next_byte_needed = 0;
    spi_byte_ready       = 0;

    IOWR(SPI_SS_BASE, 0, CAM_SS_DESELECT);
    short_delay(1000);
    spi_flush_rx();

    camera_mode_active       = 1;
    camera_capture_requested = 0;
    camera_frame_ready       = 0;
    cam_state                = CAM_RX_IDLE;
    cam_img_idx              = 0;
    cam_poll_count           = 0;

    show_status("CAMERA READY", 0xE);

    printf("CAMERA MODE: entered. Press KEY0 to capture.\n");
}

void camera_leave_mode(void)
{
    IOWR(SPI_SS_BASE, 0, CAM_SS_DESELECT);
    short_delay(1000);
    spi_flush_rx();

    camera_mode_active       = 0;
    camera_capture_requested = 0;
    camera_frame_ready       = 0;
    cam_state                = CAM_RX_IDLE;

    printf("CAMERA MODE: left.\n");
}

/*
 * Trigger one capture.
 *   1. Select camera SS.
 *   2. Clock out the command byte 0xC0.
 *   3. Move into CAM_RX_SYNC and keep clocking 0x00s in the ISR,
 *      scanning for the 0xAA 0x55 header.
 */
void camera_trigger_capture(void)
{
    if (!camera_mode_active) return;
    if (cam_state != CAM_RX_IDLE && cam_state != CAM_RX_DONE) {
        printf("CAMERA: capture already in progress\n");
        return;
    }

    show_status("CAPTURING...", 0xC);

    cam_state          = CAM_RX_SYNC;
    cam_img_idx        = 0;
    cam_poll_count     = 0;
    cam_first_rx_count = 0;
    camera_frame_ready = 0;

    spi_flush_rx();
    IOWR(SPI_SS_BASE, 0, CAM_SS_SELECT);
    short_delay(1000);

    /* Send the capture command byte. The first MISO byte clocked back
     * is whatever the ESP32 had in its tx buffer beforehand (likely 0xFF
     * or 0x00), so we ignore it via the SYNC state. */
    while (!(IORD(SPI_Status, 0) & TRDY));
    IOWR(SPI_TxData, 0, CAM_CMD_CAPTURE);

    printf("CAMERA: capture command sent.\n");
}

/*
 * SPI ISR delegate for camera mode.
 *
 * Behavior:
 *   - CAM_RX_SYNC : drop bytes until we see 0xAA, then advance.
 *   - CAM_RX_HDR1 : expect 0x55; if seen, advance to CAM_RX_IMG.
 *                   If we see another 0xAA, stay (re-sync).
 *                   Otherwise drop back to CAM_RX_SYNC.
 *   - CAM_RX_IMG  : store 784 bytes, then mark frame ready and deselect.
 *
 * After each byte, if we are still mid-frame, send another 0x00 to
 * clock the next byte from the slave.
 *
 * Gives up after CAM_MAX_POLL_BYTES filler bytes in the SYNC state, to
 * avoid wedging forever if the ESP32 never responds.
 */
void camera_spi_isr_handler(void)
{
    while (IORD(SPI_Status, 0) & RRDY) {
        uint8_t rx = (uint8_t)(IORD(SPI_RxData, 0) & 0xFF);

        /* Diagnostic: capture the first few bytes received per request,
         * so we can see what the ESP32 (or noise on MISO) is actually
         * sending back. */
        if (cam_first_rx_count < CAM_FIRST_RX_LOG) {
            cam_first_rx_log[cam_first_rx_count++] = rx;
        }

        switch (cam_state) {

        case CAM_RX_SYNC:
            cam_poll_count++;
            if (rx == CAM_HDR_0) {
                cam_state = CAM_RX_HDR1;
            } else if (cam_poll_count >= CAM_MAX_POLL_BYTES) {
                /* Time out: the ESP32 never delivered a header. */
                cam_state = CAM_RX_IDLE;
                IOWR(SPI_SS_BASE, 0, CAM_SS_DESELECT);
                printf("CAMERA: timeout waiting for header\n");
                printf("CAMERA: first %d rx bytes:", cam_first_rx_count);
                for (int k = 0; k < cam_first_rx_count; k++) {
                    printf(" %02X", (unsigned)cam_first_rx_log[k]);
                }
                printf("\n");
                return;
            }
            break;

        case CAM_RX_HDR1:
            if (rx == CAM_HDR_1) {
                cam_state   = CAM_RX_IMG;
                cam_img_idx = 0;
            } else if (rx == CAM_HDR_0) {
                cam_state = CAM_RX_HDR1;
            } else {
                cam_state = CAM_RX_SYNC;
            }
            break;

        case CAM_RX_IMG:
            cam_img_buf[cam_img_idx++] = rx;
            if (cam_img_idx >= CAM_IMG_BYTES) {
                cam_state          = CAM_RX_DONE;
                camera_frame_ready = 1;
                IOWR(SPI_SS_BASE, 0, CAM_SS_DESELECT);
                return;
            }
            break;

        case CAM_RX_IDLE:
        case CAM_RX_DONE:
        default:
            break;
        }

        /* Continue clocking while mid-transaction. */
        if (cam_state == CAM_RX_SYNC ||
            cam_state == CAM_RX_HDR1 ||
            cam_state == CAM_RX_IMG) {

            int guard = 0;
            while (!(IORD(SPI_Status, 0) & TRDY) && guard < 10000) {
                guard++;
            }
            IOWR(SPI_TxData, 0, 0x00);
        }
    }
}

void camera_render_to_dashboard(void)
{
    if (!camera_frame_ready) return;

    /* Erase the status strip so it doesn't sit on top of the image. */
    cam_fill_rect(MON_INNER_X0 + 1, MON_INNER_Y0 + 1,
                  MON_INNER_X1 - 1, MON_INNER_Y0 + 10, 0x0);

    /* Paint 28x28 -> 56x56 inside the monitor box. */
    for (int iy = 0; iy < CAM_IMG_H; iy++) {
        for (int ix = 0; ix < CAM_IMG_W; ix++) {
            unsigned char col = map_gray_to_vga(cam_img_buf[iy * CAM_IMG_W + ix]);
            int px0 = IMG_X0 + ix * IMG_SCALE;
            int py0 = IMG_Y0 + iy * IMG_SCALE;
            for (int sy = 0; sy < IMG_SCALE; sy++) {
                for (int sx = 0; sx < IMG_SCALE; sx++) {
                    cam_px(px0 + sx, py0 + sy, col);
                }
            }
        }
    }

    camera_frame_ready = 0;
    printf("CAMERA: frame rendered.\n");
}

/*
 * Diagnostic: print all 784 received grayscale bytes at a constant pace
 * so we can verify whether the SPI image data is sensible.
 * Call AFTER the frame is received (camera_frame_ready == 1).
 */
void camera_debug_print_frame(void)
{
    printf("---- CAMERA FRAME DUMP (784 bytes, 28x28) ----\n");

    for (int row = 0; row < CAM_IMG_H; row++) {
        for (int col = 0; col < CAM_IMG_W; col++) {
            int idx = row * CAM_IMG_W + col;
            printf("%3u ", (unsigned)cam_img_buf[idx]);

            /* Constant pacing per byte so the JTAG UART FIFO does
             * not overflow and so the output streams evenly. */
            short_delay(2000);
        }
        printf("\n");
        short_delay(5000);   /* extra pause between rows */
    }

    printf("---- END CAMERA FRAME DUMP ----\n");
}
