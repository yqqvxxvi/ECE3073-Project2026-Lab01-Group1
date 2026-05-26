/*
 * Core 2 - Camera test main.
 *
 * Uses helpers/globals defined in core2.c via extern.
 * Reads KEY0 from shared SDRAM (Core 1 writes it at +0x14, bit 0).
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "system.h"
#include <io.h>
#include <sys/alt_irq.h>

#include "altera_avalon_mutex.h"

#include "camera.h"
#include "dashboard.h"

/* ---------------- Externs from core2.c ---------------- */
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

/* ---------------- Shared SDRAM (Core 1 writes here) ---------------- */
#define SHARED_FLAG_BASE         ((volatile int *)(SHARED_SDRAM_BASE | 0x80000000))
#define SHARED_OFF_KEYS          0x14

/* ---------------- SPI bits ---------------- */
#define TRDY                          0x40
#define RRDY                          0x80
#define SPI_CONTROL_RRDY_INT_ENABLE   0x80

#define READ_REG(addr)        IORD(addr, 0)
#define WRITE_REG(addr, val)  IOWR(addr, 0, val)

/* ---------------- Shared SPI ISR ---------------- */
static void spi_isr(void *context, alt_u32 id)
{
    if (camera_mode_active) {
        camera_spi_isr_handler();
    } else {
        while (READ_REG(SPI_Status) & RRDY) {
            (void)(uint8_t)(READ_REG(SPI_RxData) & 0xFF);
        }
    }
}

static void spi_interrupt_init(void)
{
    spi_flush_rx();
    alt_irq_register(SPI_0_IRQ, NULL, spi_isr);
    WRITE_REG(SPI_Control, SPI_CONTROL_RRDY_INT_ENABLE);
}

/* ---------------- Shared-SDRAM key read ---------------- */
static int read_keys_shared(alt_mutex_dev *mutex)
{
    int key_word;

    if (mutex != NULL) {
        altera_avalon_mutex_lock(mutex, 2);
        key_word = IORD_16DIRECT(SHARED_FLAG_BASE, SHARED_OFF_KEYS);
        altera_avalon_mutex_unlock(mutex);
    } else {
        key_word = IORD_16DIRECT(SHARED_FLAG_BASE, SHARED_OFF_KEYS);
    }

    return key_word;
}

/* ---------------- Main ---------------- */
int main(void)
{
    printf("=== CORE 2: CAMERA TEST START ===\n");

    WRITE_REG(SPI_SS, 0x1);
    spi_flush_rx();

    spi_interrupt_init();
    printf("Core 2: SPI ISR registered.\n");

//    dashboard_init_static();
//    printf("Core 2: dashboard initialized.\n");

    camera_init();
    camera_enter_mode();

    alt_mutex_dev *mutex = altera_avalon_mutex_open("/dev/mutex");
    if (mutex == NULL) {
        printf("Core 2: WARNING - /dev/mutex open failed.\n");
    } else {
        printf("Core 2: mutex acquired handle.\n");
    }

    int prev_key0 = 0;
    int curr_key0 = 0;
    int keys      = 0;

    printf("Core 2: READY. Press KEY0 to capture.\n");

    while (1) {
        keys      = read_keys_shared(mutex);
        curr_key0 = keys & 0x1;

        if (curr_key0 == 1 && prev_key0 == 0) {
            printf("Core 2: KEY0 rising edge -> camera_trigger_capture()\n");
            camera_trigger_capture();
        }

        if (camera_frame_ready) {
            camera_debug_print_frame();
//            camera_render_to_dashboard();
        }

        prev_key0 = curr_key0;
        short_delay(5000);
    }

    return 0;
}
