// Libraries
#include <Seeed_Arduino_SSCMA.h>
#include <ArduinoJson.h>
#include <ESP32SPISlave.h>

// Note: SCLK frequency is set and supplied by master, so clock frequency is not defined here
// Must match NIOS configuration
#define SPI_MODE SPI_MODE0 // CPOL = 0, CPHA = 0

//Pin definitions for Xiao ESP32-C3
#define HSPI_MISO 9
#define HSPI_MOSI 10
#define HSPI_SCLK 8
#define HSPI_SS   1

ESP32SPISlave slave;

// Global buffers
static constexpr size_t BUFFER_SIZE = 8;   //Each SPI transaction is for 8 bytes in size
static constexpr size_t QUEUE_SIZE = 2;    //Two SPI transactions can be queued
uint8_t tx_buf[BUFFER_SIZE] {0, 0, 0, 0, 0, 0, 0, 0};   //Data to be sent to master
uint8_t rx_buf[BUFFER_SIZE] {0, 0, 0, 0, 0, 0, 0, 0};   //Where recieved data goes

void setup()
{
    Serial.begin(115200);
    delay (1000);

    slave.setDataMode(SPI_MODE);
    slave.setQueueSize(QUEUE_SIZE);
    slave.begin();
    Serial.println("SPI slave Ready!");
}

void loop()
{
    // Overwriting tx_buffer to only send 1 byte of data - this can be extended to send all 8 bytes and NIOS has to be configured accordingly
    uint8_t tx_buf[1] = {0x23};   // data to send
    //uint8_t rx_buf[1] = {0x00};
    // uint8_t tx_buf[8] = {0x23,0,0,0,0,0,0,0};

    // Send data from tx_buffer; Null - ignore receieved data; Buffer size is 1 as we are only sending 1 byte
    slave.queue(tx_buf, NULL, 1); // Slave sends data to master

    // Commented out slave recieves data from master - modify buffer size accordingly
    //slave.queue(NULL, rx_buf, BUFFER_SIZE);

    //Waiting for master with a timeout of 5 seconds - SPI is waiting for slave to be SS (or CS) to be assereted low and for SPI CLK pulses
    Serial.println("Waiting for SPI");
    const std::vector<size_t> received_bytes = slave.wait(5000);
    
    if (!received_bytes.empty())
    {
        Serial.println("Data received!");

        // Print number of bytes received
        Serial.print("Bytes received: ");
        Serial.println(received_bytes[0]);

        // Print received data
        Serial.print("Received value(s): ");
        for (size_t i = 0; i < received_bytes[0]; i++)
        {
            Serial.print("0x");
            if (rx_buf[i] < 0x10) Serial.print("0"); // formatting
            Serial.print(rx_buf[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
    else
    {
        Serial.println("Timeout - no data received");
    }

    Serial.println("--------------------");
}