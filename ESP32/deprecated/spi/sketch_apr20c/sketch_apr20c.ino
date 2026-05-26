#include <Arduino.h>
#include <ESP32SPISlave.h>

// XIAO ESP32-C3 SPI pins
#define HSPI_MISO 9
#define HSPI_MOSI 10
#define HSPI_SCLK 8
#define HSPI_SS   1

ESP32SPISlave slave;

// Sentence to send to NIOS master
const char sentence[] = "HELLO FROM ESP32";
size_t sentence_index = 0;

void setup()
{
Serial.begin(115200);
delay(1000);
slave.setDataMode(SPI_MODE0);
slave.setQueueSize(1);
slave.begin(HSPI, HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

// Print confirmation
Serial.println("SPI slave ready!");
Serial.print("SCK:  "); Serial.println(HSPI_SCLK);
Serial.print("MISO: "); Serial.println(HSPI_MISO);
Serial.print("MOSI: "); Serial.println(HSPI_MOSI);
Serial.print("SS:   "); Serial.println(HSPI_SS);
}
void loop()
{
    // Pick the current character to send (includes '\0' at end of string)
    uint8_t tx_byte = (uint8_t)sentence[sentence_index];

    // Queue exactly 1 byte for the next SPI transaction
    slave.queue(&tx_byte, NULL, 1);

    Serial.print("Queued char: ");
    if (tx_byte == '\0') {
        Serial.println("\\0");
    } else {
        Serial.print("'");
        Serial.print((char)tx_byte);
        Serial.println("'");
    }

    // Wait up to 5000 ms for the FPGA master to clock this byte out
    const std::vector<size_t> received_bytes = slave.wait(5000);

    if (!received_bytes.empty()) {
        Serial.print("Master clocked ");
        Serial.print(received_bytes[0]);
        Serial.println(" byte(s)");

        // Advance index; reset AFTER the null terminator has been sent
        if (tx_byte == '\0') {
            sentence_index = 0;
            Serial.println("Restarting sentence...");
        } else {
            sentence_index++;
        }

    } else {
        Serial.println("Timeout waiting for SPI master");
        // Do NOT advance index on timeout - retry the same byte next time
    }

    delay(10);
}