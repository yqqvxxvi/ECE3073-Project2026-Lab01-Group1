//#ifndef SPI_COMM_H
//#define SPI_COMM_H
//
//#include <stdint.h>
//
//extern volatile int *SPI_RxData;
//extern volatile int *SPI_TxData;
//extern volatile int *SPI_Status;
//extern volatile int *SPI_Control;
//extern volatile int *SPI_SS;
//
//#define TRDY 0x40
//#define RRDY 0x80
//
//void    spi_delay(void);
//void    spi_init(void);
//uint8_t spi_transfer(uint8_t tx);
//
///* Blocking receive: keeps issuing dummy bytes until a '\0' arrives or
//   buf is full. Beeps + flashes green_LED per byte (matches the working
//   prototype). max_len includes the room for the terminator. */
//void spi_receive_string(char *buf, int max_len);
//
///* Init: deassert SS and flush any stale RX byte. */
//void spi_receive_init(void);
//
///* Non-blocking helpers retained from earlier integration. */
//int spi_send_poll(const char *str, int *j, int *sending_flag);
//int spi_rx_poll(uint8_t *out);
//
//#endif
