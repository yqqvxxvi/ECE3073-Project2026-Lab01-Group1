//#include "spi_comm.h"
//#include "peri.h"
//#include <io.h>
//#include <stdio.h>
//
///* Altera SPI core register offsets (relative to SPI_0_BASE = 0x08041000) */
//#define SPI_CORE_BASE        0x08041000
//#define SPI_SLAVESEL_ADDR    (SPI_CORE_BASE + 0x14)
//
//void spi_delay(void)
//{
//    for (volatile int i = 0; i < 1000; i++);
//}
//
//void spi_init(void)
//{
//    /* Pick slave 0 in the SPI core. Without this the master will NOT
//       generate SCLK and the ESP will report "Master clocked 0 bytes". */
//    IOWR_32DIRECT((volatile int *)SPI_SLAVESEL_ADDR, 0, 0x1);
//
//    /* External SS PIO (separate wire to ESP) - keep deasserted at boot. */
//    IOWR_32DIRECT(SPI_SS, 0, 0x1);
//
//    (void)(uint8_t)IORD_32DIRECT(SPI_RxData, 0);  /* flush RX */
//}
//
//void spi_receive_init(void)
//{
//    spi_init();
//}
//
//uint8_t spi_transfer(uint8_t tx)
//{
//    uint8_t rx;
//
//    IOWR_32DIRECT(SPI_SS, 0, 0x0);
//    spi_delay();
//
//    while (!(IORD_32DIRECT(SPI_Status, 0) & TRDY));
//    IOWR_32DIRECT(SPI_TxData, 0, tx);
//
//    while (!(IORD_32DIRECT(SPI_Status, 0) & RRDY));
//    rx = (uint8_t)IORD_32DIRECT(SPI_RxData, 0);
//
//    spi_delay();
//    IOWR_32DIRECT(SPI_SS, 0, 0x1);
//
//    return rx;
//}
//
//void spi_receive_string(char *buf, int max_len)
//{
//    int index = 0;
//    uint8_t rx;
//
//    /* clear any leftover before starting */
//    spi_init();
//
//    *speaker      = 0x00;
//    *OutPort_LED8 = 0x00;
//    *green_LED    = 0x00;
//
//    printf("============================\n");
//    printf("START SPI STRING RECEIVE\n");
//
//    /* visible boot signal so user knows we entered the function */
//    *OutPort_LED8 = 0x01;
//    beep_cycles(40, 60);
//    *OutPort_LED8 = 0x00;
//
//    while (1)
//    {
//        rx = spi_transfer(0xFF);
//
//        printf("Received byte: 0x%02X", rx);
//        if (rx >= 32 && rx <= 126) printf(" ('%c')", rx);
//        printf("\n");
//
//        /* beep on EVERY byte (including 0x00) so the user can hear that
//           the SPI master is actually clocking. Helps diagnose whether
//           MISO is delivering real data or stuck low. */
//        beep_cycles(40, 60);
//
//        if (rx == '\0')
//        {
//            buf[index] = '\0';
//            printf("Full sentence: %s\n", buf);
//            printf("-------------------------\n");
//
//            /* long, obvious end-of-message indicator */
//            for (int flash = 0; flash < 4; flash++)
//            {
//                *OutPort_LED8 = 0x01;
//                *green_LED    = 0x01;
//                long_delay(800000);
//                *OutPort_LED8 = 0x00;
//                *green_LED    = 0x00;
//                long_delay(800000);
//            }
//            return;
//        }
//
//        if (index < (max_len - 1))
//        {
//            buf[index++] = (char)rx;
//            *green_LED = 0x01;
//            long_delay(100000);
//            *green_LED = 0x00;
//        }
//        else
//        {
//            buf[max_len - 1] = '\0';
//            printf("Buffer overflow, partial sentence: %s\n", buf);
//            *speaker   = 0x00;
//            *green_LED = 0x00;
//            return;
//        }
//
//        long_delay(300000);
//    }
//}
//
///* ---- non-blocking helpers (kept for possible future use) ---- */
//
//int spi_send_poll(const char *str, int *j, int *sending_flag)
//{
//    IOWR_32DIRECT(SPI_SS, 0, 0x0);
//    spi_delay();
//
//    if (str[*j] != '\0')
//    {
//        while (!(IORD_32DIRECT(SPI_Status, 0) & TRDY));
//        IOWR_32DIRECT(SPI_TxData, 0, str[*j]);
//        (*j)++;
//    }
//    else
//    {
//        while (!(IORD_32DIRECT(SPI_Status, 0) & TRDY));
//        IOWR_32DIRECT(SPI_TxData, 0, str[*j]);
//        *sending_flag = 0;
//        *j = 0;
//        spi_delay();
//        IOWR_32DIRECT(SPI_SS, 0, 0x1);
//        return 1;
//    }
//
//    spi_delay();
//    IOWR_32DIRECT(SPI_SS, 0, 0x1);
//    return 0;
//}
//
//int spi_rx_poll(uint8_t *out)
//{
//    if (IORD_32DIRECT(SPI_Status, 0) & RRDY)
//    {
//        *out = (uint8_t)IORD_32DIRECT(SPI_RxData, 0);
//        return 1;
//    }
//    return 0;
//}
