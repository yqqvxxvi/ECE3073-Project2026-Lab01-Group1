//#include "system.h"
//#include "altera_avalon_spi.h"
//#include <stdio.h>
//#include <stdint.h>
//
//#define SPI0_BASE 0X04009000
//#define SPI_SLAVE_INDEX 0  // Select SS_n[0]
//#define SPI_BUFFER_SIZE 8  // ESP32 sends 8 bytes with same predicted digit
//
//uint8_t rx_buffer[SPI_BUFFER_SIZE];
//
//void spi_receive_prediction() {
//    uint8_t dummy = 0x00;
//
//    for (int i = 0; i < SPI_BUFFER_SIZE; ++i) {
//        uint8_t received;
//
//        uint32_t flags = (i < SPI_BUFFER_SIZE - 1) ? ALT_AVALON_SPI_COMMAND_MERGE : 0;
//
//        alt_avalon_spi_command(SPI0_BASE, SPI_SLAVE_INDEX, 1, &dummy, 1, &received, flags);
//        rx_buffer[i] = received;
//        usleep(20);
//    }
//}
//
//
//int main() {
//    while (1) {
//        printf("Receiving prediction...\n");
//        spi_receive_prediction();
//
//        printf("Received bytes: ");
//        for (int i = 0; i < SPI_BUFFER_SIZE; ++i) {
//            printf("%02X ", rx_buffer[i]);
//        }
//
//        // Optional: extract prediction from first byte
//        printf(" | Prediction: %d\n", rx_buffer[0]);
//
//        usleep(1500000); // wait 1 second before next poll
//    }
//
//    return 0;
//}
